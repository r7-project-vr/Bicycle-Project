// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Animal.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/Bike/BikeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WuBranch/Actor/Component/AnimalManagerComponent.h"

// Sets default values
AAnimal::AAnimal()
	: StartChaseDistance(100.f)
	, Speed(10.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAnimal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentTarget.IsNull())
		return;

	if(!GiveUp())
		Chase(DeltaTime);
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

void AAnimal::ChangeOffset(FVector Offset)
{
	RelativeOffset = Offset;
}

void AAnimal::Chase(float DeltaTime)
{
	ACharacter* Player = CurrentTarget.Get();

	FVector PlayerLocation = Player->GetActorLocation();
	FRotator PlayerRotation = Player->GetActorRotation();
	FVector MyLocation = GetActorLocation();
	// 目標位置、プレイヤー位置 + 相対位置 * プレイヤーの角度
	FVector TargetLocation = PlayerLocation + PlayerRotation.RotateVector(RelativeOffset);
	// 目標との距離
	float Distance = FVector::DistXY(MyLocation, TargetLocation);

	// 一定以上の距離を離れたら追う
	if (Distance >= StartChaseDistance)
	{
		FVector Direction = (TargetLocation - MyLocation).GetSafeNormal();

		// 向き変更
		FRotator TargetRotation = Direction.Rotation();
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.f));
		
		// 移動
		GetCharacterMovement()->AddInputVector(Direction.GetSafeNormal() * Speed * DeltaTime);
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Direction: %s, Velocity: %s"), *Direction.ToString(), *GetCharacterMovement()->Velocity.ToString()));
	}
}

bool AAnimal::GiveUp()
{
	ACharacter* Player = CurrentTarget.Get();

	FVector PlayerLocation = Player->GetActorLocation();
	FRotator PlayerRotation = Player->GetActorRotation();
	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = PlayerLocation + PlayerRotation.RotateVector(RelativeOffset);
	float TotalDistance = FVector::DistXY(MyLocation, TargetLocation);

	// 一定以上距離を離れたら追わなくなる
	if (TotalDistance >= GiveUpDistance)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("%s distance: %lf"), *GetFName().ToString(), TotalDistance));
		AnimalManager->ReduceAnimal(this);
		CurrentTarget = nullptr;
		return true;
	}

	return false;
}

