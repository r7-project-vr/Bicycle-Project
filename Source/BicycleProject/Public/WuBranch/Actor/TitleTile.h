// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TitleTile.generated.h"

class UBoxComponent;
class UEnvironmentalObjectComponent;
class URandomFoliageSpawner;

UCLASS()
class BICYCLEPROJECT_API ATitleTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATitleTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	/// <summary>
	/// 地面
	/// </summary>
	UStaticMeshComponent* RoadMesh;

	/// <summary>
	/// 次のTileを生成する位置
	/// </summary>
	UBoxComponent* NextTileSpawnPoint;

	/// <summary>
	/// 
	/// </summary>
	UBoxComponent* TileDeletePoint;


	/// <summary>
	/// 建物のメッシュ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<UEnvironmentalObjectComponent*> Buildings;

	/// <summary>
	/// フォリッジ生成者
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	URandomFoliageSpawner* FoliageSpawner = nullptr;
};
