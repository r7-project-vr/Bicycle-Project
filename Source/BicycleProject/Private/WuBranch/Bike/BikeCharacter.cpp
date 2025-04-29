// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/BikeCharacter.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <WuBranch/MyGameInstance.h>
#include "WuBranch/Device/DeviceManager.h"
#include <WuBranch/Bike/BikeComponent.h>

// Sets default values
ABikeCharacter::ABikeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_bikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BikeMesh"));
	_bikeMesh->SetupAttachment(RootComponent);
	_bikeMesh->SetRelativeLocation(FVector(40.0f, 0.0f, -88.5f));
	_bikeMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	_bikeMesh->SetRelativeScale3D(FVector(1.4f, 1.4f, 1.4f));
	AddInstanceComponent(_bikeMesh);

	_bike = CreateDefaultSubobject<UBikeComponent>(FName("Bike"));
	AddInstanceComponent(_bike);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
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
	
	UAnimInstance* animation = GetMesh()->GetAnimInstance();

}

// Called to bind functionality to input
void ABikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetOwner()->GetWorld()->GetGameInstance());
	if (!gameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Get Game Instance Error!"));
	}
	else
	{
		UDeviceManager* deviceManager = gameInstance->GetDeviceManager();
		deviceManager->ChangeDevice(EDeviceType::UESupportDevice);
		deviceManager->BindMoveEvent(_bike, "OnMove");
	}
}

void ABikeCharacter::ChangeBikeMesh()
{

}

float ABikeCharacter::GetSpeed()
{
	return GetCharacterMovement()->Velocity.Length();
}

float ABikeCharacter::GetHandlerAngle()
{
	return 0.0f;
}

void ABikeCharacter::LoadBikeMesh()
{
	// staticMeshの方、後で削除
	/*{
		const FSoftObjectPath& assetRef = _bikeMeshNeedLoad.ToSoftObjectPath();
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(assetRef, FStreamableDelegate::CreateUObject(this, &ABikeCharacter::LoadMeshComplete));
	}*/
	
	// skeletalMesh
	if (_bikeSkeletalNeedLoad)
	{
		const FSoftObjectPath& assetRef = _bikeSkeletalNeedLoad.ToSoftObjectPath();
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(assetRef, FStreamableDelegate::CreateUObject(this, &ABikeCharacter::LoadMeshComplete));
	}
}

void ABikeCharacter::LoadMeshComplete()
{
	/*UStaticMesh* mesh = _bikeMeshNeedLoad.Get();
	if (mesh)
	{
		_bikeMesh->SetStaticMesh(mesh);
	}*/

	USkeletalMesh* skeletalMesh = _bikeSkeletalNeedLoad.Get();
	if (skeletalMesh)
	{
		GetMesh()->SetSkeletalMesh(skeletalMesh);
	}
}

