// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/BikeComponent.h"
#include "WuBranch/Device/DeviceManager.h"
#include <WuBranch/MyGameInstance.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Components/CapsuleComponent.h>
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplay.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <WuBranch/Bike/BikeCharacter.h>

// Sets default values for this component's properties
UBikeComponent::UBikeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	_speed = 50.0f;
	_isForcedControl = false;
	_inertiaDamping = 500.0f;
	_inertiaVelocity = FVector::ZeroVector;
}


// Called when the game starts
void UBikeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UBikeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	HandleInertia(DeltaTime);

	//double speed = GetOwner()->GetComponentByClass<UCharacterMovementComponent>()->Velocity.Length();
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
	GetOwner()->GetComponentByClass<UCharacterMovementComponent>()->StopMovementImmediately();
}

void UBikeComponent::HandleInertia(float DeltaTime)
{
	//UKismetSystemLibrary::PrintString(this, "inertia Velocity: " + _inertiaVelocity.ToString(), true, false, FColor::Green, 10.f);
	// 慣性で移動
	GetOwner()->AddActorWorldOffset(_inertiaVelocity);

	// 減衰
	FVector damp = _inertiaVelocity.GetSafeNormal() * _inertiaDamping * DeltaTime;
	if (damp.SizeSquared() > _inertiaVelocity.SizeSquared())
		_inertiaVelocity = FVector::ZeroVector;
	else
		_inertiaVelocity -= damp;
}

void UBikeComponent::OnMove(FVector2D direction)
{
	//UKismetSystemLibrary::PrintString(this, "Recieve Move input: " + direction.ToString(), true, false, FColor::Green, 10.f);
	if (_isForcedControl)
		return;

	// 移動方向は自転車今向いている方向を中心に
	FVector actorForward = GetOwner()->GetActorForwardVector();
	FVector actorRight = GetOwner()->GetActorRightVector();
	FVector dir;
	if (direction.X != 0.0f)
	{
		dir += actorForward * direction.X;
	}
	if (direction.Y != 0.0f)
	{
		dir += actorRight * direction.Y;
	}

	// 移動
	// AddForceで移動すると、VRの中で小さい揺れが発生して酔いやすくなるので破棄してACharacterのCharacterMovementを利用します
	ABikeCharacter* character = Cast<ABikeCharacter>(GetOwner());
	character->AddMovementInput(actorForward, dir.X);
	character->AddMovementInput(actorRight, dir.Y);

	// 慣性を設定
	_inertiaVelocity = dir.GetSafeNormal() * _speed;
}

void UBikeComponent::OnSelectLeftAnswer()
{
	HandleSelectAnswer(FRotator(0.0f, -90.0f, 0.0f));
}

void UBikeComponent::OnSelectRightAnswer()
{
	HandleSelectAnswer(FRotator(0.0f, 90.0f, 0.0f));
}

void UBikeComponent::HandleSelectAnswer(FRotator dir)
{
	// 曲がる
	GetOwner()->AddActorLocalRotation(dir);
	// 二回目以降選ばせない
	DisableSelectAnswer();
	// 曲がったら強制コントロール解除
	CloseForcedControl();
}

void UBikeComponent::DisableSelectAnswer()
{
	UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetOwner()->GetWorld()->GetGameInstance());
	UDeviceManager* deviceManager = gameInstance->GetDeviceManager();
	deviceManager->DisableSelectAnswerActions();
}

bool UBikeComponent::IsVRConnect() const
{
	return GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->XRSystem->GetHMDDevice()->IsHMDConnected();
}

