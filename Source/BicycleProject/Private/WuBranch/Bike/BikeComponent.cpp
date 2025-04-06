// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/BikeComponent.h"
#include "WuBranch/Device/DeviceManager.h"
#include "WuBranch/Device/Device.h"
#include <WuBranch/MyGameInstance.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

// Sets default values for this component's properties
UBikeComponent::UBikeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	_deviceManager = nullptr;
	_speed = 50.0f;
	_isForcedControl = false;
}


// Called when the game starts
void UBikeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetOwner()->GetWorld()->GetGameInstance());
	if (!gameInstance) 
	{
		UE_LOG(LogTemp, Error, TEXT("Get Game Instance Error!"));
	}
	else
	{
		_deviceManager = gameInstance->GetDeviceManager();
		_deviceManager->ChangeDevice(EDeviceType::Keyboard);
		//_deviceManager->ChangeDevice(EDeviceType::QuestController);
		_deviceManager->BindMoveEvent(this, "OnMove");
		//deviceManager->GetDevice()->_onMoveEvent.AddDynamic(this, &UBikeComponent::OnMove);
	}

	TArray<UActorComponent*> playerCollisions = GetOwner()->GetComponentsByTag(UCapsuleComponent::StaticClass(), FName("PlayerCollision"));
	if (playerCollisions.Num() != 0)
	{
		UCapsuleComponent* me = Cast<UCapsuleComponent>(playerCollisions[0]);
		if (me)
		{
			_player = me;
		}
	}
}

// Called every frame
void UBikeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!_deviceManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Device Manager is null!"));
		return;
	}

	//double speed = _player->GetComponentVelocity().Length();
	//UKismetSystemLibrary::PrintString(this, "Speed: " + FString::SanitizeFloat(speed), true, false, FColor::Green, 10.f);
}

void UBikeComponent::OpenForcedControl()
{
	_isForcedControl = true;
}

void UBikeComponent::CloseForcedControl()
{
	_isForcedControl = false;
}

void UBikeComponent::ReduceVelocityTo0()
{
	//double speed = _player->GetComponentVelocity().Length();
	GetOwner()->GetComponentByClass<UCharacterMovementComponent>()->StopMovementImmediately();
}

void UBikeComponent::OnMove(FVector2D direction)
{
	UKismetSystemLibrary::PrintString(this, "Recieve Move input: " + direction.ToString(), true, false, FColor::Green, 10.f);
	if (_isForcedControl)
		return;

	FVector dir(direction.X, direction.Y, 0.0f);
	// 移動
	/*TArray<UActorComponent*> playerCollisions = GetOwner()->GetComponentsByTag(UCapsuleComponent::StaticClass(), FName("PlayerCollision"));
	if (playerCollisions.Num() != 0)
	{
		UCapsuleComponent* me = Cast<UCapsuleComponent>(playerCollisions[0]);
		if (me)
		{
			me->AddForce(dir * _unitSpeed * _speed);
		}
	}*/

	TArray<UActorComponent*> bikeMesh = GetOwner()->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("BikeMesh"));
	if (bikeMesh.Num() != 0)
	{
		UStaticMeshComponent* me = Cast<UStaticMeshComponent>(bikeMesh[0]);
		if (me)
		{
			float max_speed = GetOwner()->GetComponentByClass<UCharacterMovementComponent>()->GetMaxSpeed();
			if (me->GetComponentVelocity().Length() < max_speed)
			{
				me->AddForce(dir * _unitSpeed * _speed);
			}
		}
	}
}

void UBikeComponent::OnSelectLeftAnswer()
{
	UKismetSystemLibrary::PrintString(this, "Select Left Answer", true, false, FColor::Green, 10.f);
	// 左に曲がる(今は機能しない)
	GetOwner()->AddActorLocalRotation(FRotator(0.0f, -90.0f, 0.0f));
	// 二回目以降選ばせない
	DisableSelectAnswer();
	// 曲がったら強制コントロール解除
	CloseForcedControl();
}

void UBikeComponent::OnSelectRightAnswer()
{
	UKismetSystemLibrary::PrintString(this, "Select Right Answer", true, false, FColor::Green, 10.f);
	// 右に曲がる(今は機能しない)
	GetOwner()->AddActorLocalRotation(FRotator(0.0f, 90.0f, 0.0f));
	// 二回目以降選ばせない
	DisableSelectAnswer();
	// 曲がったら強制コントロール解除
	CloseForcedControl();
}

void UBikeComponent::DisableSelectAnswer()
{
	_deviceManager->DisableSelectAnswerActions();
}

