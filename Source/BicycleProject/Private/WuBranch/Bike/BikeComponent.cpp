// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/BikeComponent.h"
#include "WuBranch/Device/DeviceManager.h"
#include "WuBranch/Device/Device.h"
#include <WuBranch/MyGameInstance.h>
#include <Kismet/KismetSystemLibrary.h>

// Sets default values for this component's properties
UBikeComponent::UBikeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	_deviceManager = nullptr;
}


// Called when the game starts
void UBikeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetOwner()->GetWorld()->GetGameInstance());
	if (!gameInstance) 
	{
		UE_LOG(LogTemp, Error, TEXT("Get Game Instance Error!"));
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, "device init start", true, false, FColor::Green, 10.f);
		_deviceManager = gameInstance->GetDeviceManager();
		_deviceManager->ChangeDevice(EDeviceType::Keyboard);
		_deviceManager->BindMoveEvent(this, "OnMove");
		//deviceManager->GetDevice()->_onMoveEvent.AddDynamic(this, &UBikeComponent::OnMove);
		UKismetSystemLibrary::PrintString(this, "bind move event complete", true, false, FColor::Green, 10.f);
	}
}

// Called every frame
void UBikeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!_deviceManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Device Manager is null!"));
		return;
	}

	IDeviceInterface* rightHand = _deviceManager->GetDevice(EDevicePart::RightHand);
	IDeviceInterface* leftHand = _deviceManager->GetDevice(EDevicePart::LeftHand);
}

void UBikeComponent::OnMove(FVector2D direction)
{
	UKismetSystemLibrary::PrintString(this, "Recieve Move input: " + direction.ToString(), true, false, FColor::Green, 10.f);
	APawn* pawn = Cast<APawn>(GetOwner());
	FVector dir(direction.X, direction.Y, 0.0f);
	pawn->AddMovementInput(dir, _speed);
}

