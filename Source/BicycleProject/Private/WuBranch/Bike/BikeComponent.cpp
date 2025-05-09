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
#include "WuBranch/UI/QuestionUIActor.h"

// Sets default values for this component's properties
UBikeComponent::UBikeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	_speed = 50.0f;
	_inertiaDamping = 10.0f;
	_inertiaVelocity = FVector::ZeroVector;
}


// Called when the game starts
void UBikeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	_isAutoPlay = false;
}

// Called every frame
void UBikeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (_isAutoPlay)
	{
		FVector deltaPos = FMath::VInterpTo(GetOwner()->GetActorLocation(), _synchronizePos, DeltaTime, 2);
		GetOwner()->SetActorLocation(deltaPos);
	}
	else
	{
		HandleInertia(DeltaTime);
	}

	//double speed = GetOwner()->GetComponentByClass<UCharacterMovementComponent>()->Velocity.Length();
	//UKismetSystemLibrary::PrintString(this, "Speed: " + FString::SanitizeFloat(speed), true, false, FColor::Green, 10.f);
}

void UBikeComponent::ReduceVelocityTo0()
{
	GetOwner()->GetComponentByClass<UCharacterMovementComponent>()->StopMovementImmediately();
}

void UBikeComponent::EnableAutoPlay(AQuestionUIActor* actor)
{
	_isAutoPlay = true;
	_questionActor = actor;
	// 慣性の力も0にする
	_inertiaVelocity = FVector::ZeroVector;
}

void UBikeComponent::DisableAutoPlay()
{
	_isAutoPlay = false;
	_questionActor = nullptr;
}

void UBikeComponent::SetSynchPos(FVector pos)
{
	_synchronizePos = pos;
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
	_questionActor->UseLeftExit();
}

void UBikeComponent::OnSelectRightAnswer()
{
	HandleSelectAnswer(FRotator(0.0f, 90.0f, 0.0f));
	_questionActor->UseRightExit();
}

void UBikeComponent::HandleSelectAnswer(FRotator dir)
{
	// 曲がる
	Cast<ABikeCharacter>(GetOwner())->SetTurningAngle(dir);
	// 二回目以降選ばせない
	DisableSelectAnswer();
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

