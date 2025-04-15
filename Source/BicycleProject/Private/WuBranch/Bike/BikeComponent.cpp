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
	_inertiaDamping = 10.0f;
	_inertiaVelocity = FVector::ZeroVector;
	_isRotate = false;
	_targetRotator = FRotator::ZeroRotator;
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

	RotateBike();

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
	FVector dir = FVector::ZeroVector;
	dir = actorForward * direction.X + actorRight * direction.Y;

	// 移動
	// AddForceで移動すると、VRの中で小さい揺れが発生して酔いやすくなるので破棄してACharacterのCharacterMovementを利用します
	ABikeCharacter* character = Cast<ABikeCharacter>(GetOwner());
	character->AddMovementInput(actorForward, direction.X);
	character->AddMovementInput(actorRight, direction.Y);

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
	_targetRotator = GetOwner()->GetActorRotation() + dir;
	_isRotate = true;
	// 二回目以降選ばせない
	DisableSelectAnswer();
}

void UBikeComponent::RotateBike()
{
	if (!_isRotate)
		return;

	FRotator current = GetOwner()->GetActorRotation();
	// 曲がった
	if (current.Equals(_targetRotator, 0.5f))
	{
		// 0.5度未満は同じと見なすため、強制的に角度を最終角度に設定します
		GetOwner()->SetActorRelativeRotation(_targetRotator);
		_isRotate = false;
		// 強制コントロール解除
		CloseForcedControl();
		return;
	}

	float deltaTime = GetOwner()->GetWorld()->GetDeltaSeconds();
	FRotator angle = FMath::RInterpTo(current, _targetRotator, deltaTime, _rotateSpeed);
	GetOwner()->SetActorRelativeRotation(angle);
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

