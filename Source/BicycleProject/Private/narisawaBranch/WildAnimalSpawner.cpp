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
	AutoSpawnDelay = 0.5f;
}

void AWildAnimalSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 自動スポーンが有効な場合
	if (bAutoSpawnOnBeginPlay)
	{
		if (AutoSpawnDelay > 0.0f)
		{
			// 遅延後にスポーン
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(
				TimerHandle,
				this,
				&AWildAnimalSpawner::SpawnAnimalsDelayed,
				AutoSpawnDelay,
				false
			);
		}
		else
		{
			// 即座にスポーン
			SpawnAnimalsDelayed();
		}
	}
}

void AWildAnimalSpawner::StartSpawnAnimals()
{
	if (WildAnimalManager)
	{
		WildAnimalManager->StartSpawnAnimal();
		UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Spawn animals requested"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[WildAnimalSpawner] WildAnimalManager is null!"));
	}
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

