// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/WiredDevice.h"
#include "ASerialLibControllerWin.h"

UWiredDevice::UWiredDevice()
	: MoveSwitch(false)
	, SelectSwitch(false)
	, Device(nullptr)
{
}

void UWiredDevice::Init(int DeviceID, int DeviceVer)
{
	Device = NewObject<UASerialLibControllerWin>();
	Device->Initialize(DeviceID, DeviceVer);
	Device->SetInterfacePt(new WindowsSerial());
}

bool UWiredDevice::Connect()
{
	if (!CheckDevice())
		return false;

	if (State == EDeviceConnectType::Connected)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("Please Disconnect first"));
		return false;
	}
	else if (State == EDeviceConnectType::Connecting)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("Please Waiting"));
		return false;
	}

	State = EDeviceConnectType::Connecting;

	ConnectResult Result = Device->AutoConnectDevice();

	if (Result == ConnectResult::Succ)
	{
		State = EDeviceConnectType::Connected;
		return true;
	}
	else
	{
		State = EDeviceConnectType::UnConnected;
		return false;
	}
}

bool UWiredDevice::Disconnect()
{
	if (!CheckDevice())
		return false;

	ConnectResult Result = Device->DisConnectDevice();

	if (Result == ConnectResult::Succ)
	{
		State = EDeviceConnectType::UnConnected;
		return true;
	}
	else
	{
		State = EDeviceConnectType::Connected;
		return false;
	}
		
}

void UWiredDevice::EnableMoveAction_Implementation()
{
	MoveSwitch = true;
}

void UWiredDevice::DisableMoveAction_Implementation()
{
	MoveSwitch = false;
}

void UWiredDevice::EnableSelectAnswerAction_Implementation()
{
	SelectSwitch = true;
}

void UWiredDevice::DisableSelectAnswerAction_Implementation()
{
	SelectSwitch = false;
}

void UWiredDevice::GetData()
{
	if (State != EDeviceConnectType::Connected)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("Device is nullptr!"));
		return;
	}
}

bool UWiredDevice::CheckDevice()
{
	if (!Device)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("Device is nullptr! Use Wired Device Init first!"));
		return false;
	}
	return true;
}
