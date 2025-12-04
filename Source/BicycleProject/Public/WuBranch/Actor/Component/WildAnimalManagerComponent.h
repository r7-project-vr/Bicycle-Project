// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WildAnimalManagerComponent.generated.h"

class UBoxComponent;
class AWildAnimal;
class ACharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UWildAnimalManagerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UWildAnimalManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Wild Animal")
	void SetSeed(int Seed);

	UFUNCTION(BlueprintCallable, Category = "Wild Animal")
	void StartSpawnAnimal();

	UFUNCTION(BlueprintCallable, Category = "Wild Animal")
	void DestroyAllAnimals();

	// 普通動物のリスト
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Spawn Settings")
	TArray<TSubclassOf<AWildAnimal>> NormalAnimalTypes;

	// レア動物のリスト
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Spawn Settings")
	TArray<TSubclassOf<AWildAnimal>> RareAnimalTypes;

	// 普通動物の重み（確率の比率）
	// デフォルト: 6 (普通動物1匹あたり6の重み)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Probability Settings", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float NormalAnimalWeight = 6.0f;

	// レア動物の重み（確率の比率）
	// デフォルト: 1 (レア動物1匹あたり1の重み)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Probability Settings", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float RareAnimalWeight = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wild Animal|Spawn Settings")
	TArray<UBoxComponent*> WildAnimalSpawnLocations;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wild Animal")
	TArray<AWildAnimal*> SpawnedAnimals;

private:
	// ランダムストリーム
	FRandomStream RandomStream;
	
	// RandomStream が初期化されたかを追跡するフラグ（インスタンス変数）
	bool bRandomStreamInitialized;

	// 普通動物とレア動物の確率テーブル
	struct FAnimalProbabilityEntry
	{
		TSubclassOf<AWildAnimal> AnimalClass;
		float CumulativeProbability; // 累積確率
	};
	TArray<FAnimalProbabilityEntry> ProbabilityTable;

	// ヘルパーメソッド
	void BuildProbabilityTable();
	void CreateAnimal(ACharacter* Character, TSubclassOf<AWildAnimal> Target, FVector Location, FRotator Rotation);
	TSubclassOf<AWildAnimal> DecideAnimal();
	
	/** RandomStream が初期化されているか確認し、必要なら初期化する */
	void EnsureRandomStreamInitialized();
};
