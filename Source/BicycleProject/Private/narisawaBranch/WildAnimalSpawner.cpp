// Fill out your copyright notice in the Description page of Project Settings.

#include "narisawaBranch/WildAnimalSpawner.h"
#include "WuBranch/Actor/Component/WildAnimalManagerComponent.h"
#include "WuBranch/Actor/WildAnimal.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"

// SceneRoot と WildAnimalManagerComponent を作成し、デフォルト設定を初期化
AWildAnimalSpawner::AWildAnimalSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// ルートコンポーネントを作成
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// WildAnimalManagerComponent を作成してルートにアタッチ
	WildAnimalManager = CreateDefaultSubobject<UWildAnimalManagerComponent>(TEXT("WildAnimalManager"));
	WildAnimalManager->SetupAttachment(RootComponent);

	// デフォルト値を設定
	bAutoSpawnOnBeginPlay = true;  // 自動スポーン有効
	AutoSpawnDelay = 1.0f;          // 1秒遅延
}

// 自動スポーンが有効な場合、遅延後にスポーンを実行
void AWildAnimalSpawner::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] BeginPlay called"));
	
	// WildAnimalManager の設定状態を確認（デバッグ用）
	if (WildAnimalManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG] WildAnimalManager.Normal Animals: %d"), WildAnimalManager->NormalAnimalTypes.Num());
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG] WildAnimalManager.Rare Animals: %d"), WildAnimalManager->RareAnimalTypes.Num());
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG] WildAnimalSpawner.Normal Animals: %d"), NormalAnimalTypes.Num());
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG] WildAnimalSpawner.Rare Animals: %d"), RareAnimalTypes.Num());
	}

	// 自動スポーンが有効な場合
	if (bAutoSpawnOnBeginPlay)
	{
		// 遅延時間を設定（最低 1.0 秒）
		// BoxComponentの初期化を待つために必要
		float DelayTime = FMath::Max(AutoSpawnDelay, 1.0f);
		
		UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Scheduling auto-spawn with delay: %.2f seconds"), DelayTime);
		
		// タイマーを設定して遅延後にスポーン
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

// 必要に応じてWildAnimalSpawnerの設定をWildAnimalManagerにコピー
void AWildAnimalSpawner::StartSpawnAnimals()
{
	if (!WildAnimalManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[WildAnimalSpawner] WildAnimalManager is null!"));
		return;
	}

	// WildAnimalManagerが既に設定されている場合は、コピーしない
	// （Blueprint で直接 WildAnimalManager に設定している場合）
	if (WildAnimalManager->NormalAnimalTypes.Num() > 0 || WildAnimalManager->RareAnimalTypes.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] WildAnimalManager already configured, skipping copy"));
	}
	// WildAnimalSpawnerに設定がある場合のみコピー
	// （BlueprintでWildAnimalSpawnerに設定している場合）
	else if (NormalAnimalTypes.Num() > 0 || RareAnimalTypes.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Copying animal settings from WildAnimalSpawner to Manager"));
		WildAnimalManager->NormalAnimalTypes = NormalAnimalTypes;
		WildAnimalManager->RareAnimalTypes = RareAnimalTypes;
		WildAnimalManager->NormalAnimalWeight = NormalAnimalWeight;
		WildAnimalManager->RareAnimalWeight = RareAnimalWeight;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[WildAnimalSpawner] Neither WildAnimalManager nor WildAnimalSpawner has animal settings!"));
	}

	// スポーン処理を実行
	UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Attempting to spawn animals..."));
	WildAnimalManager->StartSpawnAnimal();
}

// すべての動物を削除
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

// ランダムシードを設定
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

// タイマーから呼び出される内部メソッド
// 遅延後にスポーンを実行
void AWildAnimalSpawner::SpawnAnimalsDelayed()
{
	UE_LOG(LogTemp, Log, TEXT("[WildAnimalSpawner] Auto-spawning animals..."));
	StartSpawnAnimals();
}

