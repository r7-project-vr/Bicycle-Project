// Fill out your copyright notice in the Description page of Project Settings.

#include "WuBranch/Actor/Component/WildAnimalManagerComponent.h"
#include "Components/BoxComponent.h"
#include "WuBranch/Actor/WildAnimal.h"
#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UWildAnimalManagerComponent::UWildAnimalManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// コンストラクタで初期化フラグをfalseに設定
	bRandomStreamInitialized = false;
	
	// 配列をクリア（古いコンポーネント参照を削除）
	WildAnimalSpawnLocations.Empty();
}

// Called when the game starts
void UWildAnimalManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// 子階層にいるUBoxComponentを全部参照
	TArray<USceneComponent*> ChildComponents;
	GetChildrenComponents(true, ChildComponents);
	
	for (USceneComponent* Child : ChildComponents)
	{
		if (UBoxComponent* BoxComp = Cast<UBoxComponent>(Child))
		{
			WildAnimalSpawnLocations.Add(BoxComp);
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("Found %d BoxComponent(s) in child hierarchy"), WildAnimalSpawnLocations.Num());

	// RandomStream の初期化を確認
	EnsureRandomStreamInitialized();
	
	// 注意: BeginPlayでは自動スポーンしない
	// WildAnimalSpawnerなどの親アクターが明示的にStartSpawnAnimal()を呼び出す必要がある
}

void UWildAnimalManagerComponent::SetSeed(int Seed)
{
	// シード値0の場合は無視して、自動生成されたランダムシードを使用
	if (Seed == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetSeed(0) called - ignoring and using auto-generated random seed instead"));
		return;
	}
	
	RandomStream.Initialize(Seed);
	bRandomStreamInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("WildAnimalManager seed manually set to: %d"), Seed);
}

void UWildAnimalManagerComponent::StartSpawnAnimal()
{
	// RandomStream が初期化されているか確認
	EnsureRandomStreamInitialized();
	
	// 既存の動物を削除してから新しい動物をスポーン
	DestroyAllAnimals();
	
	BuildProbabilityTable();
	
	if (ProbabilityTable.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn animals: Probability table is empty!"));
		UE_LOG(LogTemp, Error, TEXT("Make sure NormalAnimalTypes and/or RareAnimalTypes are set in Blueprint!"));
		return;
	}
	
	if (WildAnimalSpawnLocations.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn animals: No spawn locations found!"));
		UE_LOG(LogTemp, Error, TEXT("Make sure BoxComponents are added as children of this component!"));
		return;
	}
	
	// スポーン場所の数をログ出力
	UE_LOG(LogTemp, Log, TEXT("=== Starting Animal Spawn with %d locations ==="), WildAnimalSpawnLocations.Num());
	
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	for(int Index = 0; Index < WildAnimalSpawnLocations.Num(); Index++)
	{
		UBoxComponent* SpawnLocation = WildAnimalSpawnLocations[Index];
		
		if(!SpawnLocation)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawn Location %d is NULL!"), Index);
			continue;
		}
		
		// 位置情報をログ出力
		FVector Location = SpawnLocation->GetComponentLocation();
		FRotator Rotation = SpawnLocation->GetComponentRotation();
		
		UE_LOG(LogTemp, Log, TEXT("Spawn Location %d - Raw Location: %s, Raw Rotation: %s"), 
			Index, *Location.ToString(), *Rotation.ToString());
		
		// Ownerの情報を確認
		AActor* Owner = GetOwner();
		if(!Owner)
		{
			UE_LOG(LogTemp, Error, TEXT("Owner is NULL! Cannot spawn animal at location %d"), Index);
			continue;
		}
		
		FVector OwnerLocation = Owner->GetActorLocation();
		FRotator OwnerRotation = Owner->GetActorRotation();
		
		UE_LOG(LogTemp, Log, TEXT("Owner Location: %s, Owner Rotation: %s"), 
			*OwnerLocation.ToString(), *OwnerRotation.ToString());
		
		// 相対位置から絶対位置への変換（変数名を変更）
		FVector LocalOffset = Location - OwnerLocation;
		FVector AdjustedLocation = OwnerRotation.RotateVector(LocalOffset) + OwnerLocation;
		FRotator AdjustedRotation = OwnerRotation + Rotation;
		
		UE_LOG(LogTemp, Log, TEXT("Adjusted Location: %s, Adjusted Rotation: %s"), 
			*AdjustedLocation.ToString(), *AdjustedRotation.ToString());
		
		TSubclassOf<AWildAnimal> SelectedAnimal = DecideAnimal();
		
		if (SelectedAnimal)
		{
			UE_LOG(LogTemp, Log, TEXT("Spawn Location %d: Selected Animal = %s"), 
				Index, *SelectedAnimal->GetName());
			
			CreateAnimal(Character, SelectedAnimal, AdjustedLocation, AdjustedRotation);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Spawn Location %d: Failed to select animal!"), Index);
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("=== Spawn Complete: %d animals spawned ==="), SpawnedAnimals.Num());
}

void UWildAnimalManagerComponent::DestroyAllAnimals()
{
	if (SpawnedAnimals.Num() <= 0)
		return;

	UE_LOG(LogTemp, Log, TEXT("Destroying %d animals"), SpawnedAnimals.Num());
	
	while (SpawnedAnimals.Num() > 0)
	{
		AWildAnimal* Animal = SpawnedAnimals.Pop();
		if (IsValid(Animal))
		{
			Animal->Destroy();
		}
	}
}

void UWildAnimalManagerComponent::EnsureRandomStreamInitialized()
{
	// インスタンスごとに初期化フラグを管理
	if (!bRandomStreamInitialized)
	{
		// ランダムシードを生成
		int32 TimeSeed = FDateTime::Now().GetTicks() % INT32_MAX;
		int32 ObjectSeed = GetTypeHash(this);
		int32 RandomComponent = FMath::Rand(); // 追加のランダム性
		int32 FinalSeed = TimeSeed ^ ObjectSeed ^ RandomComponent;
		
		RandomStream.Initialize(FinalSeed);
		bRandomStreamInitialized = true;
		
		UE_LOG(LogTemp, Log, TEXT("WildAnimalManager RandomStream initialized with seed: %d"), FinalSeed);
	}
}

void UWildAnimalManagerComponent::BuildProbabilityTable()
{
	ProbabilityTable.Empty();

	// 普通動物とレア動物の合計数を計算
	int32 TotalNormalAnimals = NormalAnimalTypes.Num();
	int32 TotalRareAnimals = RareAnimalTypes.Num();

	// 動物が設定されていない場合は処理しない
	if (TotalNormalAnimals == 0 && TotalRareAnimals == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No animals configured in WildAnimalManagerComponent"));
		return;
	}

	// 確率チェック
	if (NormalAnimalWeight <= 0.0f || RareAnimalWeight <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid weight values! NormalAnimalWeight=%.2f, RareAnimalWeight=%.2f"), 
			NormalAnimalWeight, RareAnimalWeight);
		return;
	}

	// 総確率を計算
	// NormalAnimalWeight と RareAnimalWeight は Blueprint で調整可能
	float TotalWeight = (TotalNormalAnimals * NormalAnimalWeight) + (TotalRareAnimals * RareAnimalWeight);

	if (TotalWeight <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Total weight is 0 or negative"));
		return;
	}

	float CumulativeProbability = 0.0f;

	// 普通動物を確率テーブルに追加
	float NormalAnimalProbabilityEach = (NormalAnimalWeight / TotalWeight) * 100.0f;
	for (const TSubclassOf<AWildAnimal>& AnimalClass : NormalAnimalTypes)
	{
		if (AnimalClass)
		{
			CumulativeProbability += NormalAnimalProbabilityEach;
			FAnimalProbabilityEntry Entry;
			Entry.AnimalClass = AnimalClass;
			Entry.CumulativeProbability = CumulativeProbability;
			ProbabilityTable.Add(Entry);
		}
	}

	// レア動物を確率テーブルに追加
	float RareAnimalProbabilityEach = (RareAnimalWeight / TotalWeight) * 100.0f;
	for (const TSubclassOf<AWildAnimal>& AnimalClass : RareAnimalTypes)
	{
		if (AnimalClass)
		{
			CumulativeProbability += RareAnimalProbabilityEach;
			FAnimalProbabilityEntry Entry;
			Entry.AnimalClass = AnimalClass;
			Entry.CumulativeProbability = CumulativeProbability;
			ProbabilityTable.Add(Entry);
		}
	}

	// デバッグログ出力
	UE_LOG(LogTemp, Log, TEXT("=== Wild Animal Probability Table ==="));
	UE_LOG(LogTemp, Log, TEXT("Normal Animals: %d (weight: %.2f, each %.2f%%, total %.2f%%)"), 
	       TotalNormalAnimals,
	       NormalAnimalWeight,
	       NormalAnimalProbabilityEach,
	       NormalAnimalProbabilityEach * TotalNormalAnimals);
	UE_LOG(LogTemp, Log, TEXT("Rare Animals: %d (weight: %.2f, each %.2f%%, total %.2f%%)"), 
	       TotalRareAnimals,
	       RareAnimalWeight,
	       RareAnimalProbabilityEach,
	       RareAnimalProbabilityEach * TotalRareAnimals);
	UE_LOG(LogTemp, Log, TEXT("Total: %.2f%%"), CumulativeProbability);
	UE_LOG(LogTemp, Log, TEXT("Ratio (Normal:Rare) = %.2f:%.2f"), 
	       NormalAnimalWeight, 
	       RareAnimalWeight);
	
	// 確率テーブルの内容を詳細に出力
	for (int32 i = 0; i < ProbabilityTable.Num(); i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Entry %d: %s (Cumulative: %.2f%%)"), 
			i, 
			*ProbabilityTable[i].AnimalClass->GetName(),
			ProbabilityTable[i].CumulativeProbability);
	}
}

void UWildAnimalManagerComponent::CreateAnimal(ACharacter* Character, TSubclassOf<AWildAnimal> Target, FVector Location, FRotator Rotation)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("Create Wild Animal Failed: Invalid Target Class"));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Attempting to spawn %s at %s"), 
		*Target->GetName(), *Location.ToString());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	AWildAnimal* Animal = GetWorld()->SpawnActor<AWildAnimal>(Target, Location, Rotation, SpawnParams);
	
	if (Animal)
	{
		Animal->Init(Character, nullptr);
		SpawnedAnimals.Add(Animal);
		
		UE_LOG(LogTemp, Log, TEXT("✓ Successfully spawned: %s at location %s"), 
			*Target->GetName(), 
			*Location.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("✗ Failed to spawn: %s at location %s - SpawnActor returned nullptr"), 
			*Target->GetName(), 
			*Location.ToString());
	}
}

TSubclassOf<AWildAnimal> UWildAnimalManagerComponent::DecideAnimal()
{
	if (ProbabilityTable.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Probability table is empty"));
		return nullptr;
	}

	// 0～100の範囲でランダム値を取得
	float RandomValue = RandomStream.FRandRange(0.0f, 100.0f);
	
	// デバッグログ: ランダム値を出力
	UE_LOG(LogTemp, Log, TEXT("Random Value: %.2f"), RandomValue);

	// 累積確率を使って動物を決定
	for (const FAnimalProbabilityEntry& Entry : ProbabilityTable)
	{
		if (RandomValue <= Entry.CumulativeProbability)
		{
			UE_LOG(LogTemp, Log, TEXT("Selected: %s at %.2f%%"), 
				*Entry.AnimalClass->GetName(), 
				Entry.CumulativeProbability);
			return Entry.AnimalClass;
		}
	}

	// フォールバック（通常は到達しない）
	UE_LOG(LogTemp, Warning, TEXT("Fallback: Using last animal in table"));
	return ProbabilityTable.Last().AnimalClass;
}
