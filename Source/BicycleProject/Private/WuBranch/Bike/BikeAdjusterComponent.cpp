// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/BikeAdjusterComponent.h"
#include "WuBranch/MyGameInstance.h"

// Sets default values for this component's properties
UBikeAdjusterComponent::UBikeAdjusterComponent()
	: BikeTag("BikeMesh")
	, BaseLocation(FVector::ZeroVector)
	, Offset(FVector::ZeroVector)
	, IsNeedHide(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UBikeAdjusterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Offset = LoadBikeOffset();
	Target = GetOwner()->FindComponentByTag<USceneComponent>(BikeTag);
	if (!Target)
		return;
	BaseLocation = Target->GetRelativeLocation();
	IsNeedHide = Target->bHiddenInGame;
}

// Called every frame
//void UBikeAdjusterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

bool UBikeAdjusterComponent::CanAdjust() const
{
	return Target != nullptr;
}

void UBikeAdjusterComponent::StartAdjustment()
{
	if (Target)
	{
		Target->SetHiddenInGame(false);
	}
}

void UBikeAdjusterComponent::SetHeight(float Amount)
{
	Offset.Z += Amount;
	UpdateBikeLocation();
}

void UBikeAdjusterComponent::ResetHeightOffset()
{
	Offset.Z = 0;
	UpdateBikeLocation();
}

float UBikeAdjusterComponent::GetHeight()
{
	return Offset.Z;
}

void UBikeAdjusterComponent::SetForwardLocation(float Amount)
{
	Offset.X += Amount;
	UpdateBikeLocation();
}

void UBikeAdjusterComponent::ResetForwardOffset()
{
	Offset.X = 0;
	UpdateBikeLocation();
}

float UBikeAdjusterComponent::GetForward()
{
	return Offset.X;
}

void UBikeAdjusterComponent::ResetOffset()
{
	Offset = FVector::ZeroVector;
	UpdateBikeLocation();
}

FVector UBikeAdjusterComponent::GetAdjustedLocation() const
{
	return BaseLocation + Offset;
}

void UBikeAdjusterComponent::FinishAdjustment()
{
	SaveBikeOffset();
	if (Target)
	{
		if (GetNeedHideAfter())
		{
			Target->SetHiddenInGame(true);
		}
	}
}

FVector UBikeAdjusterComponent::LoadBikeOffset()
{
	UMyGameInstance* GameInstance = GetOwner()->GetGameInstance<UMyGameInstance>();
	if (!GameInstance)
		return FVector::ZeroVector;

	return GameInstance->GetBikeOffset();
}

void UBikeAdjusterComponent::UpdateBikeLocation()
{
	if (Target)
	{
		Target->SetRelativeLocation(BaseLocation + Offset);
	}
}

bool UBikeAdjusterComponent::GetNeedHideAfter() const
{
	return IsNeedHide;
}

void UBikeAdjusterComponent::SaveBikeOffset()
{
	UMyGameInstance* GameInstance = GetOwner()->GetGameInstance<UMyGameInstance>();
	if (!GameInstance)
		return;

	GameInstance->SetBikeOffset(Offset);
}