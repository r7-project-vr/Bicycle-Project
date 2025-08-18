// Fill out your copyright notice in the Description page of Project Settings.


#include "UntakuBranch/TileManager.h"
#include "UntakuBranch/Tile.h"
#include "Engine/World.h"


// Sets default values
ATileManager::ATileManager()
	: TileCount(0)
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
		//const FName DirSocket = (PlayerChoice == 0 ? TEXT("Socket_Left") : TEXT("Socket_Right"));
		//SpawnTileAtSocket(StartingTile, DirSocket);
	}
	
}

void ATileManager::OnPlayerSteppedOnTile(ATile* NewTile)
{
	if (VisitedTiles.Contains(NewTile)) return;

	// 2025.08.01 ウー start
	VisitedTiles.Add(NewTile);
	//NewTile->SetManager(this);

	//const FName DirSocket = (PlayerChoice == 0 ? TEXT("Socket_Left") : TEXT("Socket_Right"));
	//SpawnTileAtSocket(NewTile, DirSocket);
	// 2025.08.01 ウー end

	if (VisitedTiles.Num() >= 3)
	{
		ATile* ToDelete = VisitedTiles[VisitedTiles.Num() - 3];
		// 2025.08.07 ウー start
		//if (ToDelete) ToDelete->Destroy();
		if (ToDelete) ToDelete->DestroyAll();
		// 2025.08.07 ウー end
	}
}

// 2025.08.01 ウー start
void ATileManager::SpawnNextMap(ATile* BaseTile, bool IsLeft)
{
	const FName DirSocket = (IsLeft ? TEXT("Socket_Left") : TEXT("Socket_Right"));
	SpawnTileAtSocket(BaseTile, DirSocket);
}
// 2025.08.01 ウー end

void ATileManager::SpawnTileAtSocket(ATile* BaseTile, FName DirSocket)
{
	if (!BaseTile || !TileClass) return;

	UWorld* W = GetWorld();
	if (!W) return;

	const FTransform BaseSocketWT = BaseTile->TileMesh->GetSocketTransform(DirSocket, ERelativeTransformSpace::RTS_World);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 2025.08.04 ウー start
	// BaseSocketWTのLocationやRotationは世界位置と世界角度
	FTransform Transform;
	Transform.SetLocation(BaseSocketWT.GetLocation() + BaseSocketWT.GetRotation().GetForwardVector() * 15000);
	Transform.SetRotation(BaseSocketWT.GetRotation());
	Transform.SetScale3D(FVector(1.f, 1.f, 1.f));
	
	// Tileの数をカウント
	TileCount++;

	ATile* NewTile = W->SpawnActor<ATile>(TileClass, Transform, Params);

	if (!NewTile) 
		return;

	NewTile->SetManager(this);
	NewTile->SetFoliageSeed(TileCount);

	/*const FTransform BottomLocal = Temp->TileMesh
	->GetSocketTransform(TEXT("Socket_Bottom"), ERelativeTransformSpace::RTS_Component);*/
	//Temp->Destroy();

	/*FTransform SpawnXF = BaseSocketWT * BottomLocal.Inverse();

	{
		const float AngleDeg = (DirSocket == TEXT("Socket_LEft")) ? -90.0f : +90.0f;
		const FQuat DeltaQ = FQuat(FVector::UpVector, FMath::DegreesToRadians(AngleDeg));

		const FVector Pivot = BaseSocketWT.GetLocation();

		const FVector OldLoc = SpawnXF.GetLocation();
		const FQuat OldRot = SpawnXF.GetRotation();

		const FVector NewLoc = Pivot + DeltaQ.RotateVector(OldLoc - Pivot);
		const FQuat NewRot = DeltaQ * OldRot;

		SpawnXF.SetLocation(NewLoc);
		SpawnXF.SetRotation(NewRot);
	}
	
	ATile* NewTile = W->SpawnActor<ATile>(TileClass, SpawnXF, Params);
	if (NewTile)
	{
		NewTile->SetManager(this);
		UE_LOG(LogTemp, Warning,
			TEXT("[TileManager] SpawnTileAtSocket: Base=%s Using Socket=%s -> New=%s Loc=%s Rot=%s"),
			*BaseTile->GetName(), *DirSocket.ToString(),
			*NewTile->GetName(),
			*NewTile->GetActorLocation().ToString(),
			*NewTile->GetActorRotation().ToCompactString());
	}*/
	// 2025.08.04 ウー end
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




