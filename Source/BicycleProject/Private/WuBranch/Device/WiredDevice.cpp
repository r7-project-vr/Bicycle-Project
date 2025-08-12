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
		GetMoveDataFromDevice();
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

void UWiredDevice::GetMoveDataFromDevice()
{
	if (State != EDeviceConnectType::Connected)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("Device State is not Connected, is $d"), (int)State);
		return;
	}

	// 先にデバイスにデータをもらうためのコマンドを送る
	uint8_t Command = 0x21; // RPMのコマンド
	int WriteResult = Device->WriteData(Command);
	if (WriteResult != 0)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("Failed to write data to device, result: %d"), WriteResult);
		return;
	}

	// デバイスからデータを読み取る
	int ReadResult;
	ASerialDataStruct::ASerialData ReadData;
	do {
		ReadResult = Device->ReadDataProcess(&ReadData);
	} while (ReadResult == 0);

	// データの読み取りに失敗した場合はエラーログを出力
	if (ReadResult < 0)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("Failed to read data from device"));
		return;
	}

	//　今の所、前進だけが自作デバイスを使うので、前進のデータを取得して、通知する
	int RPM = TransformDataToInt(ReadData.data, ReadData.data_num);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("RPM: %d"), RPM));
	FVector2D MoveVector(RPM, 0);
	NotifyMoveEvent(MoveVector);
}

int UWiredDevice::TransformDataToInt(const uint8_t* Data, int Size) const
{
	//RPMのデータは2バイト, Data[0]が上位バイト, Data[1]が下位バイト
	int Result = 0;
	for (int i = 0; i < Size; ++i)
	{
		Result |= (Data[i] << (8 * (Size - 1 - i)));
	}
	return Result;
}

void UWiredDevice::NotifyMoveEvent(FVector2D MoveData)
{
	if (!MoveSwitch)
		return;

	// 通知する
	if (OnMoveEvent.IsBound())
		OnMoveEvent.Broadcast(MoveData);
}
