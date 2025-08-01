// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileManager.generated.h"

class ATile;

UCLASS()
class BICYCLEPROJECT_API ATileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileManager();

	//Start tile
	UPROPERTY(EditAnywhere, Category="Tile")
	ATile* StartingTile;

	//Prefer to generated Tile Blueprint or C++ class
	UPROPERTY(EditAnywhere, Category = "Tile")
	TSubclassOf<ATile> TileClass;

	//Player option 0 = left 1 = Right, Testing stage editable
	UPROPERTY(EditAnywhere, Category = "Logic")
	int32 PlayerChoice = 0;

	//When player step into the tile, and the tile calling this
	void OnPlayerSteppedOnTile(ATile* NewTile);
	
	// 2025.08.01 ウー start
	/// <summary>
	/// マップを生成
	/// </summary>
	/// <param name="BaseTile"></param>
	/// <param name="IsLeft">true: 左の方, false: 右の方</param>
	void SpawnNextMap(ATile* BaseTile, bool IsLeft);
	// 2025.08.01 ウー end

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//Visited tiles, to delete
	TArray<ATile*> VisitedTiles;

	//Spawned Parents tiles.
	TSet<ATile*> SpawnedParents;

	//Core: Spawn method
	void SpawnChildren(ATile* BaseTile);

	void SpawnTileAtSocket(ATile* BaseTile, FName DirSocket);
	

};
