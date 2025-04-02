// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/BikeCharacter.h"
#include "Engine/AssetManager.h"

// Sets default values
ABikeCharacter::ABikeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_bike = CreateDefaultSubobject<UStaticMeshComponent>(FName("BikeMesh"));
	_bike->SetupAttachment(RootComponent);
	_bike->SetRelativeLocation(FVector(40.0f, 0.0f, -88.5f));
	_bike->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	_bike->SetRelativeScale3D(FVector(1.4f, 1.4f, 1.4f));
	AddInstanceComponent(_bike);
}

// Called when the game starts or when spawned
void ABikeCharacter::BeginPlay()
{
	Super::BeginPlay();
	LoadBikeMesh();
}

// Called every frame
void ABikeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void ABikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABikeCharacter::ChangeBikeMesh()
{

}

void ABikeCharacter::LoadBikeMesh()
{
	const FSoftObjectPath& assetRef = _bikeMesh.ToSoftObjectPath();
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(assetRef, FStreamableDelegate::CreateUObject(this, &ABikeCharacter::LoadMeshComplete));
}

void ABikeCharacter::LoadMeshComplete()
{
	UStaticMesh* mesh = _bikeMesh.Get();
	if (mesh)
	{
		_bike->SetStaticMesh(mesh);
	}
}

