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

void UWiredDevice::Tick(float DeltaTime)
{
	if (State == EDeviceConnectType::Connected)
	{
		GetDataFromDevice();
	}
}

TStatId UWiredDevice::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UWiredDevice, STATGROUP_Tickables);
}

bool UWiredDevice::IsTickableInEditor() const
{
	return false;
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

bool UWiredDevice::CheckDevice()
{
	if (!Device)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("Device is nullptr! Use Wired Device Init first!"));
		return false;
	}
	return true;
}

void UWiredDevice::GetDataFromDevice()
{
	if (State != EDeviceConnectType::Connected)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("Device State is not Connected, is $d"), (int)State);
		return;
	}

	// 先にデバイスにデータをもらうためのコマンドを送る
	int WriteResult = Device->WriteData(0x20);
	if (WriteResult != 0)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("Failed to write data to device, result: %d"), WriteResult);
		return;
	}

	// デバイスからデータを読み取る
	ASerialDataStruct::ASerialData ReadData;
	int Result = Device->ReadDataProcess(&ReadData);

	if (Result <= 0)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("Failed to read data from device, result: %d"), Result);
		return;
	}

	//　今の所、前進だけが自作デバイスを使うので、前進のデータを取得して、通知する
	UE_LOG(LogTemplateDevice, Display, TEXT("Get Data: cmd: %d, Num: %d"), ReadData.command, ReadData.data_num);
	for (uint8_t d : ReadData.data)
	{
		UE_LOG(LogTemplateDevice, Display, TEXT("Get Data: %d"), d);
	}
	UE_LOG(LogTemplateDevice, Display, TEXT("Get Data Finish"));
}