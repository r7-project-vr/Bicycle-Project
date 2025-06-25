// Fill out your copyright notice in the Description page of Project Settings.


#include "UntakuBranch/ProceduralRoadManager.h"
#include "Engine/StaticMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"



// Sets default values
AProceduralRoadManager::AProceduralRoadManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AProceduralRoadManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnInitialTile();
}

void AProceduralRoadManager::SpawnInitialTile()
{
	if (!RoadTileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RoadTileClass is not set in ProceduralRoadManager!"));
		return;
	}

	//一つ目の道路は世界原点
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	LastSpawnedTile = GetWorld()->SpawnActor<AActor>(RoadTileClass,
		FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (LastSpawnedTile)
	{
		ActiveTiles.Add(LastSpawnedTile);
	}

	
	
}

void AProceduralRoadManager::SpawnNextTile(FName ExitSocketName)
{
	if (!LastSpawnedTile || !RoadTileClass) return;

	//前の道路を取得
	UStaticMeshComponent* LastTileMeshComp = LastSpawnedTile->FindComponentByClass<UStaticMeshComponent>();
	if (!LastTileMeshComp) return;


	//新しい道路入口
	
}

