// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WildAnimalManagerComponent.generated.h"

class UBoxComponent;
class AWildAnimal;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UWildAnimalManagerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWildAnimalManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// ランダムのシードを設定する
	/// </summary>
	/// <param name="Seed">シード</param>
	void SetSeed(int Seed);

	/// <summary>
	/// 動物の生成を開始する
	/// </summary>
	void StartSpawnAnimal();

	/// <summary>
	/// すべての動物を削除
	/// </summary>
	void DestroyAllAnimals();

private:

	/// <summary>
	/// 確率の合計を計算
	/// </summary>
	void CaculateTotalProbility();

	/// <summary>
	/// 動物を生成する
	/// </summary>
	/// <param name="Character">動物の目標</param>
	/// <param name="Target">生成する動物</param>
	/// <param name="Location">座標</param>
	/// <param name="Rotation">回転</param>
	void CreateAnimal(ACharacter* Character, TSubclassOf<AWildAnimal> Target, FVector Location, FRotator Rotation);

	/// <summary>
	/// 実際生成する動物を決める
	/// </summary>
	/// <returns>動物</returns>
	TSubclassOf<AWildAnimal> DecideAnimal();

	/// <summary>
	/// 野良動物の位置
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<UBoxComponent*> WildAnimalSpawnLocations;

	/// <summary>
	/// 野良動物の種類と確率
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<AWildAnimal>, int> WildAnimalTypes;
		
	/// <summary>
	/// 確率の合計
	/// </summary>
	int TotalProbility;

	/// <summary>
	/// 生成した野良動物
	/// </summary>
	TArray<AWildAnimal*> SpawnedAnimals;

	/// <summary>
	/// ランダム生成器
	/// </summary>
	FRandomStream RandomStream;
};
