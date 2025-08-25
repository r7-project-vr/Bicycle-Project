// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/WiredDevice.h"
#include "WuBranch/Device/DeviceCmdSender.h"

#if PLATFORM_WINDOWS
UWiredDevice::UWiredDevice()
	: MoveSwitch(false)
	, Device(nullptr)
	, CmdSender(nullptr)
{
}

UWiredDevice::~UWiredDevice()
{
	if (CmdSender)
	{
		CmdSender->EnsureCompletion();
		delete CmdSender;
		CmdSender = nullptr;
	}
	if (Device)
	{
		Disconnect();
	}
}

void UWiredDevice::Init(int DeviceID, int DeviceVer)
{
	Device = NewObject<UASerialLibControllerWin>();
	Device->Initialize(DeviceID, DeviceVer);
	Device->SetInterfacePt(new WindowsSerial());
}

void UWiredDevice::Tick(float DeltaTime)
{
	if (!DataQueue.IsEmpty())
	{
		ASerialDataStruct::ASerialData RPMData;
		DataQueue.Dequeue(RPMData);
		HandleRPMData(RPMData);
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

		CmdSender = new DeviceCmdSender(Device, &CommandQueue, &DataQueue);

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
	CmdSender->ReStart();
}

void UWiredDevice::DisableMoveAction_Implementation()
{
	MoveSwitch = false;
	CmdSender->Pause();
}

void UWiredDevice::EnableSelectAnswerAction_Implementation()
{
	
}

void UWiredDevice::DisableSelectAnswerAction_Implementation()
{
	
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

	if (MoveSwitch)
	{
		// RPMコマンドを送信する
		//CommandQueue.Enqueue(0x21);
	}
}

void UWiredDevice::HandleRPMData(const ASerialDataStruct::ASerialData& RPMData)
{
	//　今の所、前進だけが自作デバイスを使うので、前進のデータを取得して、通知する
	uint16 RPM = TransformDataToInt<uint16>(RPMData.data, RPMData.data_num);
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("RPM: %d"), RPM));
	FVector2D MoveVector(RPM, 0);
	NotifyMoveEvent(MoveVector);
}

void UWiredDevice::HandleRPSData(const ASerialDataStruct::ASerialData& RPSData)
{
	int RPS = TransformDataToInt<int>(RPSData.data, RPSData.data_num);
	FVector2D MoveVector((float)RPS / 100.f, 0);
	NotifyMoveEvent(MoveVector);
}

template<typename T>
T UWiredDevice::TransformDataToInt(const uint8_t* Data, int Size) const
{
	//RPMのデータは2バイト, Data[0]が上位バイト, Data[1]が下位バイト
	T Result = 0;
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

#elif PLATFORM_ANDROID

UWiredDevice::UWiredDevice()
	: MoveSwitch(false)
{
}

UWiredDevice::~UWiredDevice()
{
}

void UWiredDevice::Init(int DeviceID, int DeviceVer)
{
}

void UWiredDevice::Tick(float DeltaTime)
{
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
	return false;
}

bool UWiredDevice::Disconnect()
{
	return false;

}

void UWiredDevice::EnableMoveAction_Implementation()
{
}

void UWiredDevice::DisableMoveAction_Implementation()
{
}

void UWiredDevice::EnableSelectAnswerAction_Implementation()
{

}

void UWiredDevice::DisableSelectAnswerAction_Implementation()
{

}

bool UWiredDevice::CheckDevice()
{
	return true;
}

void UWiredDevice::GetMoveDataFromDevice()
{
}

void UWiredDevice::NotifyMoveEvent(FVector2D MoveData)
{
	if (!MoveSwitch)
		return;

	// 通知する
	if (OnMoveEvent.IsBound())
		OnMoveEvent.Broadcast(MoveData);
}
#endif