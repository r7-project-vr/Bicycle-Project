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
	_enabled = false;

	_lineOfSight = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("LineOfSight"));
	_lineOfSight->SetupAttachment(this);
	_lineOfSight->bShowDebug = true;
}


// Called when the game starts
void UHeadLookComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (IsVRConnect())
	{
		_enabled = true;
	}
	else
	{
		// VRデバイスに接続していない場合は、HeadLookComponentを無効にする
		_enabled = false;
		DestroyComponent();
	}
}


// Called every frame
void UHeadLookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UHeadLookComponent::IsVRConnect() const
{
	return GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->XRSystem->GetHMDDevice()->IsHMDConnected();
}

