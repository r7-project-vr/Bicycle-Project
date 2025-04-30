// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/HeadLookComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplay.h"

// Sets default values for this component's properties
UHeadLookComponent::UHeadLookComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	_lineOfSight = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("LineOfSight"));
	_lineOfSight->SetupAttachment(this);
	_lineOfSight->bShowDebug = false;
	//_lineOfSight->bShowDebug = false;
	_lineOfSight->DebugColor = FColor::Orange;
}


// Called when the game starts
void UHeadLookComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (IsVRConnect())
	{
		SetActive(true);
		OpenLine();
	}
	else
	{
		// VRデバイスに接続していない場合は、HeadLookComponentを無効にする
		SetActive(false);
	}
}


// Called every frame
void UHeadLookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHeadLookComponent::OpenLine()
{
	_lineOfSight->bShowDebug = true;
}

void UHeadLookComponent::CloseLine()
{
	_lineOfSight->bShowDebug = false;
}

bool UHeadLookComponent::IsVRConnect() const
{
	return GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->XRSystem->GetHMDDevice()->IsHMDConnected();
}

