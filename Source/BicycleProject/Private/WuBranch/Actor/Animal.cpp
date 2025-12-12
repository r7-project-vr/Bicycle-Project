// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Animal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WuBranch/Actor/Component/AnimalManagerComponent.h"
#include "WuBranch/DataAssets/AnimalDataAsset.h"

// Sets default values
AAnimal::AAnimal()
	: CurrentState(BehaviorState::None)
	, Speed(10.f)
	, IsPaused(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->BodyInstance.bLockXRotation = true;
	GetCapsuleComponent()->BodyInstance.bLockYRotation = true;
	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel4);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
}

// Called when the game starts or when spawned
void AAnimal::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentState = BehaviorState::None;
	IsPaused = false;
	CurrentTarget = nullptr;
	AnimalManager = nullptr;

}

// Called every frame
void AAnimal::Tick(float DeltaTime)
{
	if(IsPaused)
		return;

	Super::Tick(DeltaTime);

	if (!CurrentTarget)
	{
		CurrentState = BehaviorState::None;
		return;
	}

	DecideBehavior();
	Action(DeltaTime);
}

void AAnimal::Pause_Implementation()
{
	IsPaused = true;
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
}

void AAnimal::ReStart_Implementation()
{
	IsPaused = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

bool AAnimal::IsPause_Implementation()
{
	return IsPaused;
}

float AAnimal::GetCurrentSpeed() const
{
	return GetCharacterMovement()->Velocity.Length();
}

void AAnimal::Init(ACharacter* Target, UAnimalManagerComponent* Manager)
{
	CurrentTarget = Target;
	AnimalManager = Manager;
}

void AAnimal::Init(ACharacter* Target, UAnimalManagerComponent* Manager, UAnimalDataAsset* Data)
{
	CurrentTarget = Target;
	AnimalManager = Manager;
	AIControllerClass = Data->AIControllerClass;
	if (Data)
	{
		ID = Data->AnimalID;
		Speed = Data->MoveSpeed;
	}
}

void AAnimal::ChangeOffset(FVector Offset)
{
	RelativeOffset = Offset;
}

int32 AAnimal::GetMyID() const
{
	return ID;
}

void AAnimal::DecideBehavior()
{
	UE_LOG(LogTemp, Warning, TEXT("Did not implement"));
}

void AAnimal::Action(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Did not implement"));
}

FVector AAnimal::GetMyTargetLocation()
{
	if(!CurrentTarget)
		return GetActorLocation();

	ACharacter* Player = CurrentTarget.Get();
	return Player->GetActorLocation();
}

FVector AAnimal::GetOffsetWorldLocation()
{
	if(!CurrentTarget)
		return GetActorLocation();

	ACharacter* Player = CurrentTarget.Get();
	FVector PlayerLocation = Player->GetActorLocation();
	FRotator PlayerRotation = Player->GetActorRotation();
	// ワールド上偏移した位置はプレイヤー位置 + 偏移量 * プレイヤーの角度
	return PlayerLocation + PlayerRotation.RotateVector(RelativeOffset);
}

void AAnimal::Move(float DeltaTime, FVector Direction)
{
	// 水平面のみ
	Direction.Z = 0;
	// 向き変更
	FRotator TargetRotation = Direction.Rotation();
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.f));

	// 移動
	GetCharacterMovement()->AddInputVector(Direction.GetSafeNormal() * Speed * DeltaTime);
}

void AAnimal::GiveUp()
{
	if(AnimalManager)
		AnimalManager->ReduceAnimal(this);
	CurrentTarget = nullptr;
}
