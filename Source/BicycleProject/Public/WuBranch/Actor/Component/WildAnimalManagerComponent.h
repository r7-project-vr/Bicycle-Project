// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WildAnimalManagerComponent.generated.h"

class UBoxComponent;
class AWildAnimal;
class ACharacter;

/**
 * 野生動物のスポーン管理コンポーネント
 * 
 * WildAnimalManagerの子コンポーネントから動物のスポーン位置を自動検出し、
 * 設定された確率に基づいて普通動物とレア動物をランダムにスポーンする。
 * BoxComponentのスケールに応じて動物のサイズを自動調整できる。
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UWildAnimalManagerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UWildAnimalManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	/// <summary>
	/// ランダム生成のシード値を設定
	/// </summary>
	/// <param name="Seed">シード値（0の場合は自動生成）</param>
	UFUNCTION(BlueprintCallable, Category = "Wild Animal")
	void SetSeed(int Seed);

	/// <summary>
	/// 動物のスポーンを開始
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Wild Animal")
	void StartSpawnAnimal();

	/// <summary>
	/// スポーンされたすべての動物を削除
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Wild Animal")
	void DestroyAllAnimals();

	/// <summary>
	/// 普通動物のリスト
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Spawn Settings")
	TArray<TSubclassOf<AWildAnimal>> NormalAnimalTypes;

	/// <summary>
	/// レア動物のリスト
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Spawn Settings")
	TArray<TSubclassOf<AWildAnimal>> RareAnimalTypes;

	/// <summary>
	/// 普通動物の確率の比率
	/// デフォルト: 6.0
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Probability Settings", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float NormalAnimalWeight = 6.0f;

	/// <summary>
	/// レア動物の確率の比率
	/// デフォルト: 1.0
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Probability Settings", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float RareAnimalWeight = 1.0f;

	/// <summary>
	/// BoxComponentのスケールに基づいて動物のスケールを自動調整するか
	/// 有効な場合、BoxComponentのスケール値がそのまま動物のスケールになる
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Scale Settings")
	bool bScaleByBoxSize = true;

	/// <summary>
	/// 動物のスケール倍率
	/// BoxComponentのスケールに対してこの倍率をかけた値が最終的な動物のスケールになる
	/// 例: BoxScale(2,2,2) × AnimalScaleMultiplier(0.5) = 動物スケール 1.0
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Scale Settings", meta = (EditCondition = "bScaleByBoxSize", ClampMin = "0.1", UIMin = "0.1"))
	float AnimalScaleMultiplier = 1.0f;

	/// <summary>
	/// 動物のスポーン位置リスト
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wild Animal|Spawn Settings")
	TArray<UBoxComponent*> WildAnimalSpawnLocations;

	/// <summary>
	/// スポーン済み動物のリスト
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wild Animal")
	TArray<AWildAnimal*> SpawnedAnimals;

private:
	/// <summary>
	/// ランダムストリーム
	/// </summary>
	FRandomStream RandomStream;
	
	/// <summary>
	/// RandomStreamが初期化済みかを示すフラグ
	/// </summary>
	bool bRandomStreamInitialized;

	/// <summary>
	/// 動物の確率テーブルエントリ
	/// </summary>
	struct FAnimalProbabilityEntry
	{
		TSubclassOf<AWildAnimal> AnimalClass;
		float CumulativeProbability;
	};
	
	/// <summary>
	/// 動物の確率テーブル
	/// </summary>
	TArray<FAnimalProbabilityEntry> ProbabilityTable;

	/// <summary>
	/// 確率テーブルを構築
	/// </summary>
	void BuildProbabilityTable();
	
	/// <summary>
	/// 動物を生成
	/// </summary>
	/// <param name="Character">プレイヤーキャラクター</param>
	/// <param name="Target">スポーンする動物のクラス</param>
	/// <param name="Location">スポーン位置</param>
	/// <param name="Rotation">スポーン時の回転</param>
	/// <param name="SpawnBox">スポーン元のBoxComponent（スケール計算用）</param>
	void CreateAnimal(ACharacter* Character, TSubclassOf<AWildAnimal> Target, FVector Location, FRotator Rotation, UBoxComponent* SpawnBox);
	
	/// <summary>
	/// ランダムに動物を決定
	/// </summary>
	/// <returns>選択された動物のクラス</returns>
	TSubclassOf<AWildAnimal> DecideAnimal();
	
	/// <summary>
	/// RandomStreamの初期化状態を確認し、必要なら初期化
	/// </summary>
	void EnsureRandomStreamInitialized();
	
	/// <summary>
	/// BoxComponentのスケールから動物のスケールを計算
	/// BoxComponentのスケール（X,Y,Zの平均）× AnimalScaleMultiplierを返す
	/// </summary>
	/// <param name="BoxComp">対象のBoxComponent</param>
	/// <returns>計算されたスケール値</returns>
	float CalculateScaleFromBoxSize(UBoxComponent* BoxComp);
};
