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

	_handlebarCenteringSpeed = 1.0f;
}

// Called when the game starts or when spawned
void ABikeCharacter::BeginPlay()
{
	Super::BeginPlay();

	LoadBikeMesh();
	_isRotate = false;
	_targetRotator = FRotator::ZeroRotator;
	_handlebarsAngle = 0.0f;
}

// Called every frame
void ABikeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UAnimInstance* animation = GetMesh()->GetAnimInstance();
	RotateBike(DeltaTime);
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
		deviceManager->BindSelectLeftEvent(_bike, "OnSelectLeftAnswer");
		deviceManager->BindSelectRightEvent(_bike, "OnSelectRightAnswer");
	}
}

void ABikeCharacter::ChangeBikeMesh()
{

}

float ABikeCharacter::GetSpeed()
{
	return GetCharacterMovement()->Velocity.Length();
}

float ABikeCharacter::GetHandlerAngle() const
{
	return _handlebarsAngle;
}

void ABikeCharacter::SetTurningAngle(FRotator angle)
{
	_targetRotator = GetActorRotation() + angle;
	// 右折
	if (angle.Yaw > 0)
		_handlebarsAngle = -30.0f;
	// 左折
	else if (angle.Yaw < 0)
		_handlebarsAngle = 30.0f;
	_isRotate = true;
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

void ABikeCharacter::RotateBike(float DeltaTime)
{
	if (!_isRotate)
		return;

	FRotator current = GetActorRotation();
	// 曲がった
	if (current.Equals(_targetRotator, 0.5f))
	{
		// 0.5度未満の時は曲がり終了と見なすため、強制的に角度を最終角度に設定します
		SetActorRelativeRotation(_targetRotator);
		_isRotate = false;
		// 強制コントロール解除
		UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
		UDeviceManager* deviceManager = gameInstance->GetDeviceManager();
		deviceManager->EnableDefaultActions();
		return;
	}

	// 自転車自身の角度の計算
	FRotator angle = FMath::RInterpTo(current, _targetRotator, DeltaTime, _rotateSpeed);
	SetActorRelativeRotation(angle);

	// ハンドルの戻り角度の計算
	_handlebarsAngle = FMath::FInterpTo(_handlebarsAngle, 0.0f, DeltaTime, _handlebarCenteringSpeed);
}

