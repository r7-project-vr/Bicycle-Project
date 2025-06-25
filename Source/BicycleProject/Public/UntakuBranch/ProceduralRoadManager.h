// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralRoadManager.generated.h"

UCLASS()
class BICYCLEPROJECT_API AProceduralRoadManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralRoadManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// /*// Called every frame
	// virtual void Tick(float DeltaTime) override;*/
	
	/** 生成したい道路のBlueprintClass */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TSubclassOf<AActor> RoadTileClass;

	/** 道路数 */
	UPROPERTY(EditAnywhere, Category = "Config")
	int32 InitialTileCount = 1;

	/** 最後生成したやつ記録、次のやつの参考 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	AActor* LastSpawnedTile;

	/**
	 * @brief 次道路を生成する関数
	 * @param ExitSocketName プレイヤ離れた最後のslot名
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnNextTile(FName ExitSocketName);

private:
	/** 初めての道路生成 */
	void SpawnInitialTile();
    
	/** 道路生成したあと記録、将来の操作に準備 */
	TArray<AActor*> ActiveTiles;

	

};
