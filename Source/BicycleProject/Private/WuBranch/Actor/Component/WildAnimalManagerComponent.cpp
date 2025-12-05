// Fill out your copyright notice in the Description page of Project Settings.

#include "WuBranch/Actor/Component/WildAnimalManagerComponent.h"
#include "Components/BoxComponent.h"
#include "WuBranch/Actor/WildAnimal.h"
#include <Kismet/GameplayStatics.h>

// 初期化フラグをfalseに設定し、スポーン位置配列をクリア
UWildAnimalManagerComponent::UWildAnimalManagerComponent()
{
	// コンストラクタで初期化フラグをfalseに設定
	bRandomStreamInitialized = false;
	
	// 配列をクリア（古いコンポーネント参照を削除）
	WildAnimalSpawnLocations.Empty();
}

// 子階層のBoxComponentを検索してスポーン位置として登録
void UWildAnimalManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[WildAnimalManager %p] BeginPlay called"), this);

	// 子階層にいるUBoxComponentを全部参照
	TArray<USceneComponent*> ChildComponents;
	GetChildrenComponents(true, ChildComponents);
	
	UE_LOG(LogTemp, Warning, TEXT("[WildAnimalManager %p] Found %d child components"), this, ChildComponents.Num());
	
	// BoxComponentのみをフィルタリングして追加
	for (USceneComponent* Child : ChildComponents)
	{
		if (UBoxComponent* BoxComp = Cast<UBoxComponent>(Child))
		{
			WildAnimalSpawnLocations.Add(BoxComp);
			UE_LOG(LogTemp, Warning, TEXT("[WildAnimalManager %p] Added BoxComponent: %s"), this, *BoxComp->GetName());
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("[WildAnimalManager %p] Found %d BoxComponent(s) in child hierarchy"), this, WildAnimalSpawnLocations.Num());

	// RandomStreamの初期化を確認
	EnsureRandomStreamInitialized();
}

// ランダムシード値を設定
// シード値 0 の場合は無視し、自動生成されたシードを使用
void UWildAnimalManagerComponent::SetSeed(int Seed)
{
	// シード値0の場合は無視して、自動生成されたランダムシードを使用
	if (Seed == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetSeed(0) called - ignoring and using auto-generated random seed instead"));
		return;
	}
	
	// 指定されたシード値で初期化
	RandomStream.Initialize(Seed);
	bRandomStreamInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("WildAnimalManager seed manually set to: %d"), Seed);
}

// 動物のスポーン処理を開始
// 確率テーブルを構築し、各スポーン位置にランダムな動物を配置
void UWildAnimalManagerComponent::StartSpawnAnimal()
{
	UE_LOG(LogTemp, Warning, TEXT("[WildAnimalManager %p] StartSpawnAnimal called with %d locations"), this, WildAnimalSpawnLocations.Num());
	
	// BoxComponent が未初期化または無効な場合は再検索
	bool bNeedsRefresh = (WildAnimalSpawnLocations.Num() == 0);
	if (!bNeedsRefresh)
	{
		// NULL チェック：配列内に無効な参照がないか確認
		for (UBoxComponent* BoxComp : WildAnimalSpawnLocations)
		{
			if (!BoxComp)
			{
				bNeedsRefresh = true;
				break;
			}
		}
	}
	
	// 必要に応じて BoxComponent を再検索
	if (bNeedsRefresh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WildAnimalManager %p] Refreshing BoxComponent list"), this);
		WildAnimalSpawnLocations.Empty();
		
		TArray<USceneComponent*> ChildComponents;
		GetChildrenComponents(true, ChildComponents);
		
		for (USceneComponent* Child : ChildComponents)
		{
			if (UBoxComponent* BoxComp = Cast<UBoxComponent>(Child))
			{
				WildAnimalSpawnLocations.Add(BoxComp);
				UE_LOG(LogTemp, Warning, TEXT("[WildAnimalManager %p] Refreshed BoxComponent: %s"), this, *BoxComp->GetName());
			}
		}
		
		UE_LOG(LogTemp, Warning, TEXT("[WildAnimalManager %p] Refreshed: Found %d BoxComponents"), this, WildAnimalSpawnLocations.Num());
	}
	
	// RandomStreamが初期化されているか確認
	EnsureRandomStreamInitialized();
	
	// 既存の動物を削除してから新しい動物をスポーン
	DestroyAllAnimals();
	
	// 確率テーブルを構築
	BuildProbabilityTable();
	
	// 確率テーブルが空の場合はエラー
	if (ProbabilityTable.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[WildAnimalManager %p] Cannot spawn animals: Probability table is empty!"), this);
		return;
	}
	
	// スポーン位置がない場合はエラー
	if (WildAnimalSpawnLocations.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[WildAnimalManager %p] Cannot spawn animals: No spawn locations found!"), this);
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("[WildAnimalManager %p] === Starting Animal Spawn with %d locations ==="), this, WildAnimalSpawnLocations.Num());
	
	// プレイヤーキャラクターを取得（動物の初期化に使用）
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	// 各スポーン位置で動物を生成
	for(int Index = 0; Index < WildAnimalSpawnLocations.Num(); Index++)
	{
		UBoxComponent* SpawnLocation = WildAnimalSpawnLocations[Index];
		
		// NULLチェック
		if(!SpawnLocation)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawn Location %d is NULL!"), Index);
			continue;
		}
		
		// スポーン位置と回転を取得
		FVector Location = SpawnLocation->GetComponentLocation();
		FRotator Rotation = SpawnLocation->GetComponentRotation();
		
		// Ownerアクターの情報を取得（座標変換に使用）
		AActor* Owner = GetOwner();
		if(!Owner)
		{
			UE_LOG(LogTemp, Error, TEXT("Owner is NULL! Cannot spawn animal at location %d"), Index);
			continue;
		}
		
		FVector OwnerLocation = Owner->GetActorLocation();
		FRotator OwnerRotation = Owner->GetActorRotation();
		
		// 相対位置から絶対位置への変換
		FVector LocalOffset = Location - OwnerLocation;
		FVector AdjustedLocation = OwnerRotation.RotateVector(LocalOffset) + OwnerLocation;
		FRotator AdjustedRotation = OwnerRotation + Rotation;
		
		// ランダムに動物を決定
		TSubclassOf<AWildAnimal> SelectedAnimal = DecideAnimal();
		
		if (SelectedAnimal)
		{
			// 動物を生成
			CreateAnimal(Character, SelectedAnimal, AdjustedLocation, AdjustedRotation);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Spawn Location %d: Failed to select animal!"), Index);
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("=== Spawn Complete: %d animals spawned ==="), SpawnedAnimals.Num());
}

// スポーンされたすべての動物を削除
void UWildAnimalManagerComponent::DestroyAllAnimals()
{
	if (SpawnedAnimals.Num() <= 0)
		return;

	UE_LOG(LogTemp, Log, TEXT("Destroying %d animals"), SpawnedAnimals.Num());
	
	// 配列から動物を取り出して破棄
	while (SpawnedAnimals.Num() > 0)
	{
		AWildAnimal* Animal = SpawnedAnimals.Pop();
		if (IsValid(Animal))
		{
			Animal->Destroy();
		}
	}
}

// RandomStream の初期化状態を確認し、必要なら初期化
// 未初期化の場合、現在時刻とオブジェクトハッシュからランダムなシード値を生成
void UWildAnimalManagerComponent::EnsureRandomStreamInitialized()
{
	// インスタンスごとに初期化フラグを管理
	if (!bRandomStreamInitialized)
	{
		// ランダムシードを生成
		// 現在時刻、オブジェクトハッシュ、FMath::Rand() を組み合わせて一意性を確保
		int32 TimeSeed = FDateTime::Now().GetTicks() % INT32_MAX;
		int32 ObjectSeed = GetTypeHash(this);
		int32 RandomComponent = FMath::Rand();
		int32 FinalSeed = TimeSeed ^ ObjectSeed ^ RandomComponent;
		
		RandomStream.Initialize(FinalSeed);
		bRandomStreamInitialized = true;
		
		UE_LOG(LogTemp, Log, TEXT("WildAnimalManager RandomStream initialized with seed: %d"), FinalSeed);
	}
}

// 確率テーブルを構築
// 普通動物とレア動物の重みから、各動物の累積確率を計算
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

	// 確率の妥当性チェック
	if (NormalAnimalWeight <= 0.0f || RareAnimalWeight <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid weight values! NormalAnimalWeight=%.2f, RareAnimalWeight=%.2f"), 
			NormalAnimalWeight, RareAnimalWeight);
		return;
	}

	// 総確率を計算
	float TotalWeight = (TotalNormalAnimals * NormalAnimalWeight) + (TotalRareAnimals * RareAnimalWeight);

	if (TotalWeight <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Total weight is 0 or negative"));
		return;
	}

	float CumulativeProbability = 0.0f;

	// 普通動物を確率テーブルに追加
	// 各普通動物の確率 = (NormalAnimalWeight / TotalWeight) * 100
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

	// デバッグログ出力：確率分布を確認
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

// 指定された位置に動物を生成
// プレイヤーキャラクターで初期化し、SpawnedAnimalsに登録
void UWildAnimalManagerComponent::CreateAnimal(ACharacter* Character, TSubclassOf<AWildAnimal> Target, FVector Location, FRotator Rotation)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("Create Wild Animal Failed: Invalid Target Class"));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Attempting to spawn %s at %s"), 
		*Target->GetName(), *Location.ToString());

	// スポーンパラメータを設定（衝突時は位置を調整）
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	// 動物をスポーン
	AWildAnimal* Animal = GetWorld()->SpawnActor<AWildAnimal>(Target, Location, Rotation, SpawnParams);
	
	if (Animal)
	{
		// プレイヤーキャラクターで初期化
		Animal->Init(Character, nullptr);
		
		// スポーン済みリストに追加
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

// 確率テーブルからランダムに動物を決定
// 累積確率を使用した重み付けランダム選択
TSubclassOf<AWildAnimal> UWildAnimalManagerComponent::DecideAnimal()
{
	if (ProbabilityTable.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Probability table is empty"));
		return nullptr;
	}

	// 0～100の範囲でランダム値を取得
	float RandomValue = RandomStream.FRandRange(0.0f, 100.0f);
	
	UE_LOG(LogTemp, Log, TEXT("Random Value: %.2f"), RandomValue);

	// 累積確率と比較して動物を決定
	// 例: RandomValue = 45.0 の場合
	// - Entry 0: BP_WildCat (累積 31.58%) → 45.0 > 31.58, 次へ
	// - Entry 1: BP_WildChicken (累積 63.16%) → 45.0 <= 63.16, 選択
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
	// RandomValue が 100.0 を超えた場合など
	UE_LOG(LogTemp, Warning, TEXT("Fallback: Using last animal in table"));
	return ProbabilityTable.Last().AnimalClass;
}
