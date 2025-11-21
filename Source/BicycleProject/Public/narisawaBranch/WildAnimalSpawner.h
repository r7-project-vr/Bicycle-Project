// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WildAnimalSpawner.generated.h"

class UWildAnimalManagerComponent;
class USceneComponent;

/**
 * 野生動物のスポーン専用アクター
 * ResultTestマップなどで動物を生成するために使用
 */
UCLASS()
class BICYCLEPROJECT_API AWildAnimalSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AWildAnimalSpawner();

protected:
	virtual void BeginPlay() override;

public:
	/** 動物のスポーンを開始 */
	UFUNCTION(BlueprintCallable, Category = "Wild Animal Spawner")
	void StartSpawnAnimals();

	/** すべての動物を削除 */
	UFUNCTION(BlueprintCallable, Category = "Wild Animal Spawner")
	void DestroyAllAnimals();

	/** ランダムシードを設定 */
	UFUNCTION(BlueprintCallable, Category = "Wild Animal Spawner")
	void SetRandomSeed(int32 Seed);

protected:
	/** ルートコンポーネント */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	/** 動物マネージャーコンポーネント */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWildAnimalManagerComponent* WildAnimalManager;

	/** BeginPlayで自動的にスポーンするか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal Spawner|Settings")
	bool bAutoSpawnOnBeginPlay;

	/** 自動スポーン時の遅延時間（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal Spawner|Settings", meta = (EditCondition = "bAutoSpawnOnBeginPlay", ClampMin = "0.0", UIMin = "0.0"))
	float AutoSpawnDelay;

private:
	/** 遅延後に動物をスポーン */
	void SpawnAnimalsDelayed();
};
