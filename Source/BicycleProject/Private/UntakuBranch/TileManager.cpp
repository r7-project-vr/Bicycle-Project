// Fill out your copyright notice in the Description page of Project Settings.


#include "UntakuBranch/TileManager.h"
#include "UntakuBranch/Tile.h"
#include "Engine/World.h"


// Sets default values
ATileManager::ATileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ATileManager::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[TileManager] BeginPlay"));
		
	
	
	if (StartingTile && TileClass)
	{
		// Pin the Start tile
		VisitedTiles.Add(StartingTile);
		StartingTile->SetManager(this);
		//Create Left Right tiles
		SpawnChildren(StartingTile);
	}
	
}

void ATileManager::OnPlayerSteppedOnTile(ATile* NewTile)
{
	if (VisitedTiles.Contains(NewTile)) return;

	VisitedTiles.Add(NewTile);
	NewTile->SetManager(this);

	SpawnChildren(NewTile);

	//Delete 2 Steps pre tile
	if (VisitedTiles.Num() >= 3)
	{
		ATile* ToDelete = VisitedTiles[VisitedTiles.Num() - 3];
		if (ToDelete) ToDelete->Destroy();
	}
}

void ATileManager::SpawnTileAtSocket(ATile* BaseTile, FName DirSocket)
{
	if (!BaseTile || !TileClass) return;

	UWorld* W = GetWorld();
	if (!W) return;

	const FTransform BaseSocketWT = BaseTile->TileMesh->GetSocketTransform(DirSocket, ERelativeTransformSpace::RTS_World);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATile* Temp = W->SpawnActor<ATile>(TileClass, FTransform::Identity, Params);
	if (!Temp) return;

	const FTransform BottomLocal = Temp->TileMesh
	->GetSocketTransform(TEXT("Socket_Bottom"), ERelativeTransformSpace::RTS_Component);
	Temp->Destroy();

	const FTransform SpawnXF = BaseSocketWT * BottomLocal.Inverse();

	ATile* NewTile = W->SpawnActor<ATile>(TileClass, SpawnXF, Params);
	if (NewTile)
	{
		NewTile->SetManager(this);
		UE_LOG(LogTemp, Warning,
			TEXT("[TileManager] SpawnTileAtSocket: Base=%s Using Socket=%s -> New=%s Position=%s"),
			*BaseTile->GetName(), *DirSocket.ToString(),
			*NewTile->GetName(), *NewTile->GetActorLocation().ToString());
	}
	
}


void ATileManager::SpawnChildren(ATile* BaseTile)
{
	if (SpawnedParents.Contains(BaseTile)) return;
	SpawnedParents.Add(BaseTile);

	UWorld* W = GetWorld();
	FName Sockets[2] = { TEXT("Socket_Left"), TEXT("Socket_Right") };

	for (int32 i = 0; i < 2; ++i)
	{
		//PlayerChoice single side spawn
		FName DirSocket = Sockets[i];

		//1. Get BaseTile Socket world Transform
		FTransform BaseWT = BaseTile->TileMesh->GetSocketTransform(DirSocket, ERelativeTransformSpace::RTS_World);

		//2. Spawn Tile (for a while), get bottom socket's transform
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ATile* Temp = W->SpawnActor<ATile>(TileClass, FTransform::Identity, Params);
		if (!Temp) continue;

		FTransform BottomLocal = Temp->TileMesh->GetSocketTransform(TEXT("Socket_Bottom"), ERelativeTransformSpace::RTS_Component);

		Temp->Destroy();

		//3. caculating new Tile 's transform
		FTransform SpawnXF = BaseWT * BottomLocal.Inverse();

		//4. Real Spawn
		ATile* NewTile = W->SpawnActor<ATile>(TileClass, SpawnXF, Params);
		if (NewTile)
		{
			NewTile->SetManager(this);
		}
		
	}
}




