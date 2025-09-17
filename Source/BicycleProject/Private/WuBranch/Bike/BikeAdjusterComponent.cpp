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
	USceneComponent* SceneComp = GetOwner()->FindComponentByTag<USceneComponent>(BikeTag);
	if (!SceneComp)
		return;
	BaseLocation = SceneComp->GetRelativeLocation();
	IsNeedHide = SceneComp->bHiddenInGame;
}

// Called every frame
//void UBikeAdjusterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UBikeAdjusterComponent::SetHeight(float Amount)
{
	Offset.Z += Amount;
}

void UBikeAdjusterComponent::SetForwardLocation(float Amount)
{
	Offset.X += Amount;
}

void UBikeAdjusterComponent::ResetOffset()
{
	Offset = FVector::ZeroVector;
}

FVector UBikeAdjusterComponent::GetAdjustedLocation() const
{
	return BaseLocation + Offset;
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

FVector UBikeAdjusterComponent::LoadBikeOffset()
{
	UMyGameInstance* GameInstance = GetOwner()->GetGameInstance<UMyGameInstance>();
	if (!GameInstance)
		return FVector::ZeroVector;

	return GameInstance->GetBikeOffset();
}
