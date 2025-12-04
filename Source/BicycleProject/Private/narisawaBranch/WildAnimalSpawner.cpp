// Fill out your copyright notice in the Description page of Project Settings.

#include "narisawaBranch/WildAnimalSpawner.h"
#include "WuBranch/Actor/Component/WildAnimalManagerComponent.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"

AWildAnimalSpawner::AWildAnimalSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// ルートコンポーネントを作成
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// WildAnimalManagerComponentを作成
	WildAnimalManager = CreateDefaultSubobject<UWildAnimalManagerComponent>(TEXT("WildAnimalManager"));
	WildAnimalManager->SetupAttachment(RootComponent);

	// デフォルト値を設定
	bAutoSpawnOnBeginPlay = true;
	AutoSpawnDelay = 1.0f; // 0.5秒から1.0秒に変更
}

void AWildAnimalSpawner::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] BeginPlay called"));

	// 自動スポーンが有効な場合
	if (bAutoSpawnOnBeginPlay)
	{
		// Blueprint設定が確実に適用されるまで待つため、最低でも1.0秒遅延させる
		float DelayTime = FMath::Max(AutoSpawnDelay, 1.0f);
		
		UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Scheduling auto-spawn with delay: %.2f seconds"), DelayTime);
		
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(
			TimerHandle,
			this,
			&AWildAnimalSpawner::SpawnAnimalsDelayed,
			DelayTime,
			false
		);
	}
}

void AWildAnimalSpawner::StartSpawnAnimals()
{
	if (!WildAnimalManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[WildAnimalSpawner] WildAnimalManager is null!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Attempting to spawn animals..."));
	
	// デバッグ: 動物設定を確認
	UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Normal Animal Types Count: %d"), 
		WildAnimalManager->NormalAnimalTypes.Num());
	UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Rare Animal Types Count: %d"), 
		WildAnimalManager->RareAnimalTypes.Num());
	
	WildAnimalManager->StartSpawnAnimal();
}

void AWildAnimalSpawner::DestroyAllAnimals()
{
	if (WildAnimalManager)
	{
		WildAnimalManager->DestroyAllAnimals();
		UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Destroy all animals requested"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[WildAnimalSpawner] WildAnimalManager is null!"));
	}
}

void AWildAnimalSpawner::SetRandomSeed(int32 Seed)
{
	if (WildAnimalManager)
	{
		WildAnimalManager->SetSeed(Seed);
		UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Random seed set to: %d"), Seed);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[WildAnimalSpawner] WildAnimalManager is null!"));
	}
}

void AWildAnimalSpawner::SpawnAnimalsDelayed()
{
	UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Auto-spawning animals..."));
	StartSpawnAnimals();
}

