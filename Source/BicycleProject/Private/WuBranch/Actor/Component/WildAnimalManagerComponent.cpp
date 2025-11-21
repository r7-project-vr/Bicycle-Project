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

	// ...
	for(int Index = 0; Index < 5; Index++)
	{
		UBoxComponent* BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName(*FString::Printf(TEXT("Wild Animal Spawn Location%d"), Index)));
		if(BoxComponent)
		{
			BoxComponent->SetupAttachment(this);
			WildAnimalSpawnLocations.Add(BoxComponent);
		}
	}
	
	// コンストラクタで初期化フラグをfalseに設定
	bRandomStreamInitialized = false;
}

// Called when the game starts
void UWildAnimalManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// RandomStream の初期化を確認
	EnsureRandomStreamInitialized();
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
		return;
	}
	
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	UE_LOG(LogTemp, Log, TEXT("=== Starting Animal Spawn ==="));
	
	for(int Index = 0; Index < WildAnimalSpawnLocations.Num(); Index++)
	{
		if(WildAnimalSpawnLocations[Index])
		{
			FVector Location = WildAnimalSpawnLocations[Index]->GetComponentLocation();
			FRotator Rotation = WildAnimalSpawnLocations[Index]->GetComponentRotation();
			FVector OnwerLocation = GetOwner()->GetActorLocation();
			FRotator OnwerRotation = GetOwner()->GetActorRotation();

			FVector AdjustedLocation = OnwerRotation.RotateVector(Location) + OnwerLocation;
			FRotator AdjustedRotation = OnwerRotation + Rotation;
			
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

	AWildAnimal* Animal = GetWorld()->SpawnActor<AWildAnimal>(Target, Location, Rotation);
	if (Animal)
	{
		Animal->Init(Character, nullptr);
		SpawnedAnimals.Add(Animal);
		
		UE_LOG(LogTemp, Log, TEXT("Successfully spawned: %s at location %s"), 
			*Target->GetName(), 
			*Location.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn: %s at location %s"), 
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
