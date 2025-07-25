// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/DeviceManager.h"
#include <WuBranch/Device/KeyboardDevice.h>
#include <Kismet/KismetSystemLibrary.h>

void UDeviceManager::AddDevice(EDevicePart part, TScriptInterface<IDeviceInterface> device)
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

IDeviceInterface* UDeviceManager::GetDevice(EDevicePart part)
{
	TScriptInterface<IDeviceInterface>* device = Devices.Find(part);
	if (device)
		return device->GetInterface();
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
		break;
	default:
		FString typeName = UEnum::GetDisplayValueAsText(type).ToString();
		UE_LOG(LogTemplateDevice, Error, TEXT("The constructor method for this device type (%s) has not been implemented yet."), *typeName);
		break;
	}
}

void UDeviceManager::EnableDefaultActions()
{
	IDeviceInterface::Execute_EnableDefaultActions(Device);
}

void UDeviceManager::DisableDefaultActions()
{
	IDeviceInterface::Execute_DisableDefaultActions(Device);
}

void UDeviceManager::EnableSelectAnswerActions()
{
	IDeviceInterface::Execute_EnableSelectAnswerActions(Device);
}

void UDeviceManager::DisableSelectAnswerActions()
{
	IDeviceInterface::Execute_DisableSelectAnswerActions(Device);
}

void UDeviceManager::BindMoveEvent(UObject* object, FName functionName)
{
	IDeviceInterface::Execute_BindMoveEvent(Device, object, functionName);
}

void UDeviceManager::BindSelectLeftEvent(UObject* object, FName functionName)
{
	IDeviceInterface::Execute_BindSelectLeftEvent(Device, object, functionName);
}

void UDeviceManager::BindSelectRightEvent(UObject* object, FName functionName)
{
	IDeviceInterface::Execute_BindSelectRightEvent(Device, object, functionName);
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

void UDeviceManager::CreateQuestControllerDevice()
{
}
