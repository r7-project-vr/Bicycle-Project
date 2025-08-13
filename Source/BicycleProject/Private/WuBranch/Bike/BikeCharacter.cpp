// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/BikeCharacter.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <WuBranch/MyGameInstance.h>
#include "WuBranch/Device/DeviceManager.h"
#include <WuBranch/Bike/BikeComponent.h>
#include "WuBranch/Bike/WidgetInteractionHeadComponent.h"
#include <WuBranch/QuestionGameMode.h>

// Sets default values
ABikeCharacter::ABikeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_bike = CreateDefaultSubobject<UBikeComponent>(FName("Bike"));
	AddInstanceComponent(_bike);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	_handlebarCenteringSpeed = 1.0f;
	_isRotate = false;
	_handlebarsAngle = 0.0f;
}

// Called when the game starts or when spawned
void ABikeCharacter::BeginPlay()
{
	Super::BeginPlay();

	// カメラの下に置いたヒントラインを取得
	_widgetInteractionHeadComponent = GetComponentByClass<UWidgetInteractionHeadComponent>();
	// 自転車のメッシュをロード
	LoadBikeMesh();
	//_isRotate = false;
	_targetRotator = FRotator::ZeroRotator;
	//_handlebarsAngle = 0.0f;
}

// Called every frame
void ABikeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RotateBike(DeltaTime);
}

// Called to bind functionality to input
void ABikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (!gameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Get Game Instance Error!"));
	}
	else
	{
		UDeviceManager* deviceManager = gameInstance->GetDeviceManager();
		deviceManager->CreateAllDevices();
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

void ABikeCharacter::DisableHintLine()
{
	if (_widgetInteractionHeadComponent)
	{
		_widgetInteractionHeadComponent->DisableHintLine();
	}
}

UBikeComponent* ABikeCharacter::GetBikeComponent()
{
	return _bike;
}

void ABikeCharacter::LoadBikeMesh()
{	
	if (_bikeSkeletalNeedLoad)
	{
		const FSoftObjectPath& assetRef = _bikeSkeletalNeedLoad.ToSoftObjectPath();
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(assetRef, FStreamableDelegate::CreateUObject(this, &ABikeCharacter::LoadMeshComplete));
	}
}

void ABikeCharacter::LoadMeshComplete()
{
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
		_handlebarsAngle = 0.0f;
		_isRotate = false;
		// 強制コントロール解除、その前にゲームオーバーしたかどうかを確認する
		if (!Cast<AQuestionGameMode>(GetWorld()->GetAuthGameMode())->IsGameFailed())
		{
			UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
			UDeviceManager* deviceManager = gameInstance->GetDeviceManager();
			deviceManager->EnableDefaultActions();
		}
		return;
	}

	// 自転車自身の角度の計算
	FRotator angle = FMath::RInterpTo(current, _targetRotator, DeltaTime, _rotateSpeed);
	SetActorRelativeRotation(angle);

	// ハンドルの戻り角度の計算
	_handlebarsAngle = FMath::FInterpTo(_handlebarsAngle, 0.0f, DeltaTime, _handlebarCenteringSpeed);
}

