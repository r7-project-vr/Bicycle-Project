// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RandomFoliageSpawner.generated.h"

class UFoliageType_InstancedStaticMesh;
class UInstancedStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API URandomFoliageSpawner : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URandomFoliageSpawner();

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
	/// フォリッジの生成を開始する
	/// </summary>
	void StartSpawnFoliage();

	/// <summary>
	/// フォリッジを削除する
	/// </summary>
	void RemoveFoliageInstances();

private:

	/// <summary>
	/// フォリッジのタイプを初期化する
	/// </summary>
	void InitializeFoliageTypes();

	/// <summary>
	/// 指定範囲内でランダムにフォリッジを生成する
	/// </summary>
	void SpawnFoliageInZone();

	/// <summary>
	/// フォリッジを指定位置に追加する
	/// </summary>
	/// <param name="InLocation">位置</param>
	/// <param name="Zone">指定範囲</param>
	void AddFoliageInstance(const FVector InLocation, const FBox& Zone);

	/// <summary>
	/// 他の木も含めて、フォリッジのクラスターを生成する
	/// </summary>
	/// <param name="FoliageType"></param>
	/// <param name="ClusterLocation"></param>
	void SpawnFoliageCluster(UFoliageType_InstancedStaticMesh* FoliageType, UInstancedStaticMeshComponent* FoliageIsmComponent, const FVector& ClusterLocation, const FBox& Zone);

	/// <summary>
	/// ランダムの単位ベクトルを取得する(Z軸は含まない)
	/// </summary>
	/// <returns>単位ベクトル</returns>
	FVector GetRandomUnitVector();

	/// <summary>
	/// 指定範囲内にいるですか？
	/// </summary>
	/// <param name="Location">今の位置</param>
	/// <param name="Zone">指定範囲</param>
	/// <returns>true: はい, false: いいえ</returns>
	bool IsInZone2D(const FVector& Location, const FBox& Zone);

	/// <summary>
	/// 木のメッシュ
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings", meta = (AllowPrivateAccess = "true"))
	TArray<UFoliageType_InstancedStaticMesh*> FoliageTypes;
	
	/// <summary>
	/// 各フォリッジのインスタンス
	/// </summary>
	TArray<UInstancedStaticMeshComponent*> FoliageIsmComponents;

	/// <summary>
	/// ランダム生成器の初期シード
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings", meta = (AllowPrivateAccess = "true"))
	int InitialSeed = 0;

	/// <summary>
	/// ランダム生成器
	/// </summary>
	UPROPERTY(BlueprintReadOnly, Category = "Foliage Settings", meta = (AllowPrivateAccess = "true"))
	FRandomStream RandomStream;

	/// <summary>
	/// 指定範囲
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Foliage Settings", meta = (AllowPrivateAccess = "true"))
	TArray<FBox> SpawnZones;

	/// <summary>
	/// 生成する数
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings", meta = (AllowPrivateAccess = "true"))
	int32 NumberOfInstancesPerZone = 100;

	/// <summary>
	/// 角度(最小、最大)
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings", Meta = (AllowPrivateAccess = "true"))
	FVector2D RandomYawRotation = FVector2D(0.0f, 360.0f);

	/// <summary>
	/// 大きさ(最小、最大)
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings", Meta = (AllowPrivateAccess = "true"))
	FVector2D RandomScale = FVector2D(0.8f, 1.2f);
};
