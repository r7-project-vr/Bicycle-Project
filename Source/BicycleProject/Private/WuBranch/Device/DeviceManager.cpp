// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/DeviceManager.h"
#include <WuBranch/Device/KeyboardDevice.h>
#include <Kismet/KismetSystemLibrary.h>
#include <WuBranch/Device/WiredDevice.h>

void UDeviceManager::AddDevice(EDevicePart part, UDevice* device)
{
	if (!Devices.Contains(part))
	{
		Devices.Add(part, device);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("This device already exists, so this operation will be ignored. If you want to replace the device, please delete it first and then add a new one."));
	}
}

void UDeviceManager::DeleteDevice(EDevicePart part)
{
	if (!Devices.Contains(part))
	{
		UE_LOG(LogTemp, Warning, TEXT("This device cannot be found, so this operation will be ignored."));
	}
	else
	{
		Devices.Remove(part);
	}
}

UDevice* UDeviceManager::GetDevice(EDevicePart part)
{
	UDevice* device = *Devices.Find(part);
	if (device)
		return device;
	else
		return nullptr;
}

void UDeviceManager::ChangeDevice(EDeviceType type)
{
	switch (type)
	{
	case EDeviceType::UESupportDevice:
		CreateKeyBoardDevice();
		break;
	case EDeviceType::CustomDevice:
		CreateWiredDevice();
		break;
	default:
		FString typeName = UEnum::GetDisplayValueAsText(type).ToString();
		UE_LOG(LogTemplateDevice, Error, TEXT("The constructor method for this device type (%s) has not been implemented yet."), *typeName);
		break;
	}
}

void UDeviceManager::EnableDefaultActions()
{
	IMoveProvider::Execute_EnableMoveAction(Device);
}

void UDeviceManager::DisableDefaultActions()
{
	IMoveProvider::Execute_DisableMoveAction(Device);
}

void UDeviceManager::EnableSelectAnswerActions()
{
	IChoiceProvider::Execute_EnableSelectAnswerAction(Device);
}

void UDeviceManager::DisableSelectAnswerActions()
{
	IChoiceProvider::Execute_DisableSelectAnswerAction(Device);
}

void UDeviceManager::BindMoveEvent(UObject* object, FName functionName)
{
	IMoveProvider::Execute_BindMoveEvent(Device, object, functionName);
}

void UDeviceManager::BindSelectLeftEvent(UObject* object, FName functionName)
{
	IChoiceProvider::Execute_BindSelectLeftEvent(Device, object, functionName);
}

void UDeviceManager::BindSelectRightEvent(UObject* object, FName functionName)
{
	IChoiceProvider::Execute_BindSelectRightEvent(Device, object, functionName);
}

UDevice* UDeviceManager::GetDevice()
{
	return Device;
}

void UDeviceManager::CreateKeyBoardDevice()
{
	Device = NewObject<UKeyboardDevice>(this);
	Device->Init();
}

void UDeviceManager::CreateWiredDevice()
{
	Device = NewObject<UWiredDevice>(this);
	int DeviceID = 0x03; // Example Device ID
	int DeviceVer = 0x01; // Example Device Version
	Device->Init(DeviceID, DeviceVer);
	bool Result = Device->Connect();
	if (Result)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Wired Device Connect Success"));
		EnableDefaultActions();
	}
}
