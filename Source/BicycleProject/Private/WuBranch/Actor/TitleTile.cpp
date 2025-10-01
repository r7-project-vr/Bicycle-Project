// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/TitleTile.h"
#include "WuBranch/Actor/Component/RandomFoliageSpawner.h"

// Sets default values
ATitleTile::ATitleTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RoadMesh = CreateDefaultSubobject<UStaticMeshComponent>("TileMesh");
	RootComponent = RoadMesh;

	FoliageSpawner = CreateDefaultSubobject<URandomFoliageSpawner>("Foliage Spawner");
	FoliageSpawner->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATitleTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATitleTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

