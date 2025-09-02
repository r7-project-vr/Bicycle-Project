// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Animal.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/Bike/BikeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	
	Target = Cast<ACharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ABikeCharacter::StaticClass()));
}

// Called every frame
void AAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Chase(DeltaTime);
}

float AAnimal::GetCurrentSpeed() const
{
	return GetCharacterMovement()->Velocity.Length();
}

void AAnimal::Chase(float DeltaTime)
{
	if (Target.IsNull())
		return;

	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = Target.Get()->GetActorLocation();
	float TotalDistance = FVector::DistXY(MyLocation, TargetLocation);

	// 一定以上の距離を離れたら追う
	if (TotalDistance >= StartChaseDistance)
	{
		FVector Direction = TargetLocation - MyLocation;

		// 向き変更
		SetActorRotation(Direction.Rotation());
		// 座標
		GetCharacterMovement()->AddInputVector(Direction.GetSafeNormal() * Speed * DeltaTime);
		//SetActorLocation(GetActorLocation() + Direction.GetSafeNormal() * DeltaDistance);
	}
}

