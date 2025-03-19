// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Device/DeviceManager.h"

UMyGameInstance::UMyGameInstance()
{
	deviceManager = nullptr;
}

void UMyGameInstance::Init()
{
	Super::Init();

	if (!deviceManager)
	{
		deviceManager = NewObject<UDeviceManager>(this);
	}
}

UDeviceManager* UMyGameInstance::GetDeviceManager() const
{
	return deviceManager;
}
