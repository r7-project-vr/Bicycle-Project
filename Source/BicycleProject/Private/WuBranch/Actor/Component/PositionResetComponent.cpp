// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/PositionResetComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/Bike/BikeCharacter.h"

// Sets default values for this component's properties
UPositionResetComponent::UPositionResetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
	
}


// Called when the game starts
void UPositionResetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	TArray<AActor*> FoundActors;
	bool bHasFound = false;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		APlayerStart* MyPlayerStart = Cast<APlayerStart>(Actor);
		if (MyPlayerStart)
		{
			bHasFound = true;
			Point.SetLocation(MyPlayerStart->GetActorLocation());
			Point.SetRotation(MyPlayerStart->GetActorQuat());
		}
	}
	if (!bHasFound)
	{
		Point.SetLocation(FVector::ZeroVector);
		Point.SetRotation(FQuat::Identity);
	}
	Point.SetScale3D(FVector::OneVector);
}

// Called every frame
//void UPositionResetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UPositionResetComponent::RecordResetPosition(FVector Pos)
{
	Point.SetLocation(Pos);
}

void UPositionResetComponent::RecordResetAngle(FQuat Angle)
{
	Point.SetRotation(Angle);
}

FTransform UPositionResetComponent::GetResetPoint() const
{
	return Point;
}

void UPositionResetComponent::ResetOwnerTransform()
{
	if (AActor* Owner = GetOwner())
	{
		// 移動しなくなり
		if (ABikeCharacter* Bike = Cast<ABikeCharacter>(Owner))
			Bike->StopMove();
		// 位置、向きを変更
		Owner->SetActorLocation(Point.GetLocation());
		Owner->SetActorRotation(Point.GetRotation());
	}
}
