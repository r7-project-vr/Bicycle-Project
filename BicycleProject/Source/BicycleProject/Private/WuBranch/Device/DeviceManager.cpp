// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/DeviceManager.h"

void UDeviceManager::AddDevice(EDeviceType type, TScriptInterface<IDeviceInterface> device)
{
	if (!devices.Contains(type))
	{
		devices.Add(type, device);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("This device already exists, so this operation will be ignored. If you want to replace the device, please delete it first and then add a new one."));
	}
}

void UDeviceManager::DeleteDevice(EDeviceType type)
{
	if (!devices.Contains(type))
	{
		UE_LOG(LogTemp, Warning, TEXT("This device cannot be found, so this operation will be ignored."));
	}
	else
	{
		devices.Remove(type);
	}
}

IDeviceInterface* UDeviceManager::GetDevice(EDeviceType type)
{
	TScriptInterface<IDeviceInterface>* device = devices.Find(type);
	if (device)
		return device->GetInterface();
	else
		return nullptr;
}
