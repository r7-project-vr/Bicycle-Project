// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WildAnimalSpawner.generated.h"

class UWildAnimalManagerComponent;
class USceneComponent;
class AWildAnimal;

/**
 * 野生動物のスポーン専用アクター
 * 
 * ResultTest マップなどで動物を生成するために使用。
 * 
 * 使用方法:
 * 1. レベルに配置
 * 2. WildAnimalManagerで動物の種類と確率を設定(初期値はBPで設定)
 * 3. BeginPlay で自動的にスポーン、または手動で StartSpawnAnimals を呼び出し
 */
UCLASS()
class BICYCLEPROJECT_API AWildAnimalSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	/// <summary>
	/// コンストラクタ
	/// SceneRootとWildAnimalManagerComponentを作成し、
	/// デフォルト設定を初期化
	/// </summary>
	AWildAnimalSpawner();

protected:
	/// <summary>
	/// ゲーム開始時の処理
	/// 自動スポーンが有効な場合、指定された遅延後に動物をスポーン
	/// </summary>
	virtual void BeginPlay() override;

public:
	/// <summary>
	/// 動物のスポーンを開始
	/// WildAnimalManager に動物の設定をコピー（必要な場合）してから、
	/// スポーン処理を実行
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Wild Animal Spawner")
	void StartSpawnAnimals();

	/// <summary>
	/// すべての動物を削除
	/// WildAnimalManagerに削除を委譲
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Wild Animal Spawner")
	void DestroyAllAnimals();

	/// <summary>
	/// ランダムシードを設定
	/// 動物の出現パターンを再現可能にする
	/// </summary>
	/// <param name="Seed">シード値</param>
	UFUNCTION(BlueprintCallable, Category = "Wild Animal Spawner")
	void SetRandomSeed(int32 Seed);

protected:
	/// <summary>
	/// ルートコンポーネント
	/// このアクターの基準点となるシーンコンポーネント
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	/// <summary>
	/// 動物マネージャーコンポーネント
	/// 実際の動物のスポーン処理を管理
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWildAnimalManagerComponent* WildAnimalManager;

	/// <summary>
	/// BeginPlayで自動的にスポーンするか
	/// trueの場合、ゲーム開始時に自動的に動物をスポーンする
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal Spawner|Settings")
	bool bAutoSpawnOnBeginPlay;

	/// <summary>
	/// 自動スポーン時の遅延時間（秒）
	/// BeginPlayからスポーンまでの待機時間を指定します。
	/// BoxComponentの初期化を待つために最低1.0 秒に設定されます。
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal Spawner|Settings", meta = (EditCondition = "bAutoSpawnOnBeginPlay", ClampMin = "0.0", UIMin = "0.0"))
	float AutoSpawnDelay;

	/// <summary>
	/// 普通動物のリスト（WildAnimalSpawnerで設定する場合）
	/// WildAnimalManagerに設定がない場合、この配列の内容がコピーされます
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal Spawner|Animal Settings")
	//TArray<TSubclassOf<AWildAnimal>> NormalAnimalTypes;
	TArray<int32> NormalAnimalTypes;

	/// <summary>
	/// レア動物のリスト（WildAnimalSpawner で設定する場合）
	/// WildAnimalManagerに設定がない場合、この配列の内容がコピーされます
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal Spawner|Animal Settings")
	//TArray<TSubclassOf<AWildAnimal>> RareAnimalTypes;
	TArray<int32> RareAnimalTypes;

	/// <summary>
	/// 普通動物の重み
	/// WildAnimalManagerに設定がない場合、この値がコピーされます
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal Spawner|Animal Settings", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float NormalAnimalWeight = 6.0f;

	/// <summary>
	/// レア動物の重み
	/// WildAnimalManagerに設定がない場合、この値がコピーされます
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal Spawner|Animal Settings", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float RareAnimalWeight = 1.0f;

private:
	/// <summary>
	/// 遅延後に動物をスポーン
	/// タイマーから呼び出され、StartSpawnAnimalsを実行します
	/// </summary>
	void SpawnAnimalsDelayed();
};
