// Fill out your copyright notice in the Description page of Project Settings.

#include "WuBranch/Actor/Component/WildAnimalManagerComponent.h"
#include "Components/BoxComponent.h"
#include "WuBranch/Actor/WildAnimal.h"
#include <Kismet/GameplayStatics.h>
#include <Engine/AssetManager.h>
#include <WuBranch/DataAssets/AnimalDataAsset.h>

// 初期化フラグをfalseに設定し、スポーン位置配列をクリア
UWildAnimalManagerComponent::UWildAnimalManagerComponent()
{
	bRandomStreamInitialized = false;
	WildAnimalSpawnLocations.Empty();
}

// 子階層のBoxComponentを検索してスポーン位置として登録
void UWildAnimalManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// BoxComponentのみをフィルタリングして追加
	if (WildAnimalSpawnLocations.Num() == 0)
	{
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
	}
	
	UE_LOG(LogTemp, Log, TEXT("[WildAnimalManager %p] Found %d BoxComponent(s)"), this, WildAnimalSpawnLocations.Num());

	// RandomStreamの初期化を確認
	EnsureRandomStreamInitialized();
}

// ランダムシード値を設定
void UWildAnimalManagerComponent::SetSeed(int Seed)
{
	if (Seed == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetSeed(0) called - using auto-generated seed"));
		return;
	}
	
	RandomStream.Initialize(Seed);
	bRandomStreamInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("WildAnimalManager seed set to: %d"), Seed);
}

// 動物のスポーン処理を開始
void UWildAnimalManagerComponent::StartSpawnAnimal()
{
	// BoxComponentが未初期化の場合は再検索
	bool bNeedsRefresh = (WildAnimalSpawnLocations.Num() == 0);
	if (!bNeedsRefresh)
	{
		for (UBoxComponent* BoxComp : WildAnimalSpawnLocations)
		{
			if (!BoxComp)
			{
				bNeedsRefresh = true;
				break;
			}
		}
	}
	
	if (bNeedsRefresh)
	{
		WildAnimalSpawnLocations.Empty();
		TArray<USceneComponent*> ChildComponents;
		GetChildrenComponents(true, ChildComponents);
		
		for (USceneComponent* Child : ChildComponents)
		{
			if (UBoxComponent* BoxComp = Cast<UBoxComponent>(Child))
			{
				WildAnimalSpawnLocations.Add(BoxComp);
			}
		}
	}
	
	EnsureRandomStreamInitialized();
	DestroyAllAnimals();
	BuildProbabilityTable();
	
	if (ProbabilityTable.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn: Probability table is empty!"));
		return;
	}
	
	if (WildAnimalSpawnLocations.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn: No spawn locations!"));
		return;
	}

	TArray<FPrimaryAssetId> AnimalIDs = GetAllAnimalID();
	LoadedAnimalCount = AnimalIDs.Num();
	for (const FPrimaryAssetId& ID : AnimalIDs)
	{
		UAssetManager::Get().LoadPrimaryAsset(ID, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UWildAnimalManagerComponent::OnAnimalLoaded, ID));
	}
	
	//ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	//
	//for(int Index = 0; Index < WildAnimalSpawnLocations.Num(); Index++)
	//{
	//	UBoxComponent* SpawnLocation = WildAnimalSpawnLocations[Index];
	//	
	//	if(!SpawnLocation)
	//	{
	//		continue;
	//	}
	//	
	//	FVector Location = SpawnLocation->GetComponentLocation();
	//	FRotator Rotation = SpawnLocation->GetComponentRotation();
	//	
	//	AActor* Owner = GetOwner();
	//	if(!Owner)
	//	{
	//		continue;
	//	}
	//	
	//	FVector OwnerLocation = Owner->GetActorLocation();
	//	FRotator OwnerRotation = Owner->GetActorRotation();
	//	
	//	// 相対位置から絶対位置への変換
	//	FVector LocalOffset = Location - OwnerLocation;
	//	FVector AdjustedLocation = OwnerRotation.RotateVector(LocalOffset) + OwnerLocation;
	//	FRotator AdjustedRotation = OwnerRotation + Rotation;
	//	
	//	TSubclassOf<AWildAnimal> SelectedAnimal = DecideAnimal();
	//	
	//	if (SelectedAnimal)
	//	{
	//		// SpawnLocationを渡してスケール計算できるようにする
	//		CreateAnimal(Character, SelectedAnimal, AdjustedLocation, AdjustedRotation, SpawnLocation);
	//	}
	//}
	//
	//UE_LOG(LogTemp, Log, TEXT("Spawn Complete: %d animals spawned"), SpawnedAnimals.Num());
}

// スポーンされたすべての動物を削除
void UWildAnimalManagerComponent::DestroyAllAnimals()
{
	if (SpawnedAnimals.Num() <= 0)
		return;

	while (SpawnedAnimals.Num() > 0)
	{
		AWildAnimal* Animal = SpawnedAnimals.Pop();
		if (IsValid(Animal))
		{
			Animal->Destroy();
		}
	}
}

// RandomStreamの初期化状態を確認し、必要なら初期化
void UWildAnimalManagerComponent::EnsureRandomStreamInitialized()
{
	if (!bRandomStreamInitialized)
	{
		int32 TimeSeed = FDateTime::Now().GetTicks() % INT32_MAX;
		int32 ObjectSeed = GetTypeHash(this);
		int32 RandomComponent = FMath::Rand();
		int32 FinalSeed = TimeSeed ^ ObjectSeed ^ RandomComponent;
		
		RandomStream.Initialize(FinalSeed);
		bRandomStreamInitialized = true;
		
		UE_LOG(LogTemp, Log, TEXT("RandomStream initialized with seed: %d"), FinalSeed);
	}
}

// 確率テーブルを構築
void UWildAnimalManagerComponent::BuildProbabilityTable()
{
	ProbabilityTable.Empty();

	int32 TotalNormalAnimals = NormalAnimalTypes.Num();
	int32 TotalRareAnimals = RareAnimalTypes.Num();

	if (TotalNormalAnimals == 0 && TotalRareAnimals == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No animals configured"));
		return;
	}

	if (NormalAnimalWeight <= 0.0f || RareAnimalWeight <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid weight values!"));
		return;
	}

	float TotalWeight = (TotalNormalAnimals * NormalAnimalWeight) + (TotalRareAnimals * RareAnimalWeight);

	if (TotalWeight <= 0.0f)
	{
		return;
	}

	float CumulativeProbability = 0.0f;

	// 普通動物を確率テーブルに追加
	float NormalAnimalProbabilityEach = (NormalAnimalWeight / TotalWeight) * 100.0f;
	/*for (const TSubclassOf<AWildAnimal>& AnimalClass : NormalAnimalTypes)
	{
		if (AnimalClass)
		{
			CumulativeProbability += NormalAnimalProbabilityEach;
			FAnimalProbabilityEntry Entry;
			Entry.AnimalClass = AnimalClass;
			Entry.CumulativeProbability = CumulativeProbability;
			ProbabilityTable.Add(Entry);
		}
	}*/
	for (const int32& AnimalID : NormalAnimalTypes)
	{
		if (AnimalID > -1)
		{
			CumulativeProbability += NormalAnimalProbabilityEach;
			FAnimalProbabilityEntry Entry;
			Entry.AnimalID = AnimalID;
			Entry.CumulativeProbability = CumulativeProbability;
			ProbabilityTable.Add(Entry);
		}
	}


	// レア動物を確率テーブルに追加
	float RareAnimalProbabilityEach = (RareAnimalWeight / TotalWeight) * 100.0f;
	//for (const TSubclassOf<AWildAnimal>& AnimalClass : RareAnimalTypes)
	//{
	//	if (AnimalClass)
	//	{
	//		CumulativeProbability += RareAnimalProbabilityEach;
	//		FAnimalProbabilityEntry Entry;
	//		//Entry.AnimalClass = AnimalClass;
	//		Entry.CumulativeProbability = CumulativeProbability;
	//		ProbabilityTable.Add(Entry);
	//	}
	//}
	for (const int32& AnimalID : RareAnimalTypes)
	{
		if (AnimalID > -1)
		{
			CumulativeProbability += RareAnimalProbabilityEach;
			FAnimalProbabilityEntry Entry;
			Entry.AnimalID = AnimalID;
			Entry.CumulativeProbability = CumulativeProbability;
			ProbabilityTable.Add(Entry);
		}
	}
}

// BoxComponentのスケールから動物のスケールを計算
// BoxComponentのScale（X,Y,Zの平均）× AnimalScaleMultiplier を返す
float UWildAnimalManagerComponent::CalculateScaleFromBoxSize(UBoxComponent* BoxComp)
{
	// スケール調整が無効、またはBoxComponentがnullの場合はデフォルトスケール
	if (!bScaleByBoxSize || !BoxComp)
	{
		return 1.0f;
	}

	// BoxComponentのスケールを取得
	// GetComponentScale() はコンポーネントのワールドスケールを返す
	FVector BoxScale = BoxComp->GetComponentScale();
	
	// X, Y, Zの平均スケールを計算
	float AverageScale = (BoxScale.X + BoxScale.Y + BoxScale.Z) / 3.0f;
	
	// AnimalScaleMultiplier を適用
	float FinalScale = AverageScale * AnimalScaleMultiplier;
	
	UE_LOG(LogTemp, Log, TEXT("BoxScale: %s, AverageScale: %.2f, Multiplier: %.2f, FinalScale: %.2f"), 
		*BoxScale.ToString(), AverageScale, AnimalScaleMultiplier, FinalScale);
	
	return FinalScale;
}

// 指定された位置に動物を生成
void UWildAnimalManagerComponent::CreateAnimal(ACharacter* Character, TSubclassOf<AWildAnimal> Target, FVector Location, FRotator Rotation, UBoxComponent* SpawnBox)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Target Class"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	AWildAnimal* Animal = GetWorld()->SpawnActor<AWildAnimal>(Target, Location, Rotation, SpawnParams);
	
	if (Animal)
	{
		// BoxComponentのサイズに基づいてスケールを設定
		float Scale = CalculateScaleFromBoxSize(SpawnBox);
		Animal->SetActorScale3D(FVector(Scale, Scale, Scale));
		
		// プレイヤーキャラクターで初期化
		Animal->Init(Character, nullptr, nullptr);
		
		// スポーン済みリストに追加
		SpawnedAnimals.Add(Animal);
		
		UE_LOG(LogTemp, Log, TEXT("Spawned: %s at %s with scale %.2f"), 
			*Target->GetName(), *Location.ToString(), Scale);
	}
}

void UWildAnimalManagerComponent::CreateAnimal(ACharacter* Character, TSubclassOf<AWildAnimal> Target, FVector Location, FRotator Rotation, UBoxComponent* SpawnBox, UAnimalDataAsset* Data)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Target Class"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AWildAnimal* Animal = GetWorld()->SpawnActor<AWildAnimal>(Target, Location, Rotation, SpawnParams);

	if (Animal)
	{
		// プレイヤーキャラクターで初期化
		Animal->Init(Character, nullptr, Data);

		// BoxComponentのサイズに基づいてスケールを設定
		float Scale = CalculateScaleFromBoxSize(SpawnBox);
		Animal->SetActorScale3D(FVector(Scale, Scale, Scale));

		// スポーン済みリストに追加
		SpawnedAnimals.Add(Animal);

		UE_LOG(LogTemp, Log, TEXT("Spawned: %s at %s with scale %.2f"),
			*Target->GetName(), *Location.ToString(), Scale);
	}
}

// 確率テーブルからランダムに動物を決定
int32 UWildAnimalManagerComponent::DecideAnimal()
{
	if (ProbabilityTable.Num() == 0)
	{
		//return nullptr;
		return -1;
	}

	float RandomValue = RandomStream.FRandRange(0.0f, 100.0f);

	for (const FAnimalProbabilityEntry& Entry : ProbabilityTable)
	{
		if (RandomValue <= Entry.CumulativeProbability)
		{
			//return Entry.AnimalClass;
			return Entry.AnimalID;
		}
	}

	//return ProbabilityTable.Last().AnimalClass;
	return ProbabilityTable.Last().AnimalID;
}

TArray<FPrimaryAssetId> UWildAnimalManagerComponent::GetAllAnimalID() const
{
	TArray<FPrimaryAssetId> AnimalIDs;
	UAssetManager::Get().GetPrimaryAssetIdList(FPrimaryAssetType("AnimalData"), AnimalIDs);
	return AnimalIDs;
}

void UWildAnimalManagerComponent::OnAnimalLoaded(FPrimaryAssetId LoadedId)
{
	UAssetManager& AM = UAssetManager::Get();
	UObject* LoadedObj = AM.GetPrimaryAssetObject(LoadedId);
	if (UAnimalDataAsset* Data = Cast<UAnimalDataAsset>(LoadedObj))
	{
		AnimalDataMap.Add(Data->AnimalID, Data);
	}
	LoadedAnimalCount--;

	if (LoadedAnimalCount == 0)
	{
		OnLoadAnimalCompleted();
	}
}

void UWildAnimalManagerComponent::OnLoadAnimalCompleted()
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	for (int Index = 0; Index < WildAnimalSpawnLocations.Num(); Index++)
	{
		UBoxComponent* SpawnLocation = WildAnimalSpawnLocations[Index];

		if (!SpawnLocation)
		{
			continue;
		}

		FVector Location = SpawnLocation->GetComponentLocation();
		FRotator Rotation = SpawnLocation->GetComponentRotation();

		AActor* Owner = GetOwner();
		if (!Owner)
		{
			continue;
		}

		//FVector OwnerLocation = Owner->GetActorLocation();
		//FRotator OwnerRotation = Owner->GetActorRotation();
		
		// 相対位置から絶対位置への変換
		//FVector LocalOffset = Location - OwnerLocation;
		//FVector AdjustedLocation = OwnerRotation.RotateVector(LocalOffset) + OwnerLocation;
		//FRotator AdjustedRotation = OwnerRotation + Rotation;
		FVector AdjustedLocation = Location;
		FRotator AdjustedRotation = Rotation;
		
		int32 SelectedAnimalID = DecideAnimal();

		//if (SelectedAnimal)
		//{
		//	// SpawnLocationを渡してスケール計算できるようにする
		//	CreateAnimal(Character, SelectedAnimal, AdjustedLocation, AdjustedRotation, SpawnLocation);
		//}
		if (AnimalDataMap.Contains(SelectedAnimalID))
		{
			UAnimalDataAsset* Data = AnimalDataMap[SelectedAnimalID];
			if(WildSample)
				CreateAnimal(Character, WildSample, AdjustedLocation, AdjustedRotation, SpawnLocation, Data);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Spawn Complete: %d animals spawned"), SpawnedAnimals.Num());
}