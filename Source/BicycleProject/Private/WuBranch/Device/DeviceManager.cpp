// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/DeviceManager.h"
#include <WuBranch/Device/KeyboardDevice.h>
#include <WuBranch/Device/QuestControllerDevice.h>
#include <Kismet/KismetSystemLibrary.h>

void UDeviceManager::AddDevice(EDevicePart part, TScriptInterface<IDeviceInterface> device)
{
	if (!_devices.Contains(part))
	{
		_devices.Add(part, device);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("This device already exists, so this operation will be ignored. If you want to replace the device, please delete it first and then add a new one."));
	}
}

void UDeviceManager::DeleteDevice(EDevicePart part)
{
	if (!_devices.Contains(part))
	{
		UE_LOG(LogTemp, Warning, TEXT("This device cannot be found, so this operation will be ignored."));
	}
	else
	{
		_devices.Remove(part);
	}
}

IDeviceInterface* UDeviceManager::GetDevice(EDevicePart part)
{
	TScriptInterface<IDeviceInterface>* device = _devices.Find(part);
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
	IDeviceInterface::Execute_EnableDefaultActions(_device);
}

void UDeviceManager::DisableDefaultActions()
{
	IDeviceInterface::Execute_DisableDefaultActions(_device);
}

void UDeviceManager::EnableSelectAnswerActions()
{
	IDeviceInterface::Execute_EnableSelectAnswerActions(_device);
}

void UDeviceManager::DisableSelectAnswerActions()
{
	IDeviceInterface::Execute_DisableSelectAnswerActions(_device);
}

void UDeviceManager::BindMoveEvent(UObject* object, FName functionName)
{
	IDeviceInterface::Execute_BindMoveEvent(_device, object, functionName);
}

void UDeviceManager::BindSelectLeftEvent(UObject* object, FName functionName)
{
	IDeviceInterface::Execute_BindSelectLeftEvent(_device, object, functionName);
}

void UDeviceManager::BindSelectRightEvent(UObject* object, FName functionName)
{
	IDeviceInterface::Execute_BindSelectRightEvent(_device, object, functionName);
}

UDevice* UDeviceManager::GetDevice()
{
	return _device;
}

void UDeviceManager::CreateKeyBoardDevice()
{
	_device = NewObject<UKeyboardDevice>(this);
	_device->Init();
}

void UDeviceManager::CreateQuestControllerDevice()
{
	_device = NewObject<UQuestControllerDevice>(this);
	_device->Init();
}
