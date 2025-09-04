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
	

	//CurrentTarget = Cast<ACharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ABikeCharacter::StaticClass()));
	//if(CurrentTarget)
	//	TargetPreLocation = CurrentTarget->GetActorLocation();
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

void AAnimal::Chase(float DeltaTime)
{
	ACharacter* Target = CurrentTarget.Get();
	FVector MyLocation = GetActorLocation();
	FVector TargetCurrentLocation = Target->GetActorLocation();
	float TotalDistance = FVector::DistXY(MyLocation, TargetCurrentLocation);

	// 一定以上の距離を離れたら追う
	if (TotalDistance >= StartChaseDistance)
	{
		FVector Direction = TargetCurrentLocation - MyLocation;

		// 向き変更
		SetActorRotation(Direction.Rotation());
		
		// 座標
		GetCharacterMovement()->AddInputVector(Direction.GetSafeNormal() * Speed * DeltaTime);
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Direction: %s, Velocity: %s"), *Direction.ToString(), *GetCharacterMovement()->Velocity.ToString()));
	}

	TargetPreLocation = TargetCurrentLocation;
}

bool AAnimal::GiveUp()
{
	ACharacter* Target = CurrentTarget.Get();
	FVector MyLocation = GetActorLocation();
	FVector TargetCurrentLocation = Target->GetActorLocation();
	float TotalDistance = FVector::DistXY(MyLocation, TargetCurrentLocation);

	// 一定以上距離を離れたら追わなくなる
	if (TotalDistance >= GiveUpDistance)
	{
		AnimalManager->ReduceAnimal(this);
		CurrentTarget = nullptr;
		return true;
	}

	return false;
}

