// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/BikeComponent.h"
#include "WuBranch/Device/DeviceManager.h"
#include "WuBranch/Device/Device.h"
#include <WuBranch/MyGameInstance.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Components/CapsuleComponent.h>

// Sets default values for this component's properties
UBikeComponent::UBikeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	_deviceManager = nullptr;
	_speed = 50.0f;
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
		_deviceManager = gameInstance->GetDeviceManager();
		_deviceManager->ChangeDevice(EDeviceType::Keyboard);
		_deviceManager->BindMoveEvent(this, "OnMove");
		//deviceManager->GetDevice()->_onMoveEvent.AddDynamic(this, &UBikeComponent::OnMove);
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
}

void UBikeComponent::OnMove(FVector2D direction)
{
	UKismetSystemLibrary::PrintString(this, "Recieve Move input: " + direction.ToString(), true, false, FColor::Green, 10.f);
	FVector dir(direction.X, direction.Y, 0.0f);
	// 移動
	UCapsuleComponent* me = GetOwner()->GetComponentByClass<UCapsuleComponent>();
	me->AddForce(dir * _unitSpeed * _speed);
}

