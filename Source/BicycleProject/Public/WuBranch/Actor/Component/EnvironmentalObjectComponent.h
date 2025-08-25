// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "EnvironmentalObjectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UEnvironmentalObjectComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnvironmentalObjectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// 建物の生成を開始する
	/// </summary>
	void StartSpawnEnvironmentalObject();

private:

	/// <summary>
	/// 確率の合計を計算
	/// </summary>
	void CaculateTotalProbility();

	/// <summary>
	/// リソースのロード
	/// </summary>
	/// <param name="Target">対象物</param>
	void LoadResource(TSoftObjectPtr<UStaticMesh> Target);

	/// <summary>
	/// 建物を生成する
	/// </summary>
	/// <param name="Target">生成する目標</param>
	void CreateBuilding(UStaticMesh* Target);

	/// <summary>
	/// 実際生成するメッシュを決める
	/// </summary>
	/// <returns></returns>
	TSoftObjectPtr<UStaticMesh> DecideMesh();

	/// <summary>
	/// 環境物(建物とか)とその確率
	/// </summary>
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//TMap<UStaticMesh*, int> EnvironmentalList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<TSoftObjectPtr<UStaticMesh>, int> EnvironmentalList;

	/// <summary>
	/// 確率の合計
	/// </summary>
	int TotalProbility;

	/// <summary>
	/// 環境物
	/// </summary>
	class AStaticMeshActor* EnvironmentalObject;
};
