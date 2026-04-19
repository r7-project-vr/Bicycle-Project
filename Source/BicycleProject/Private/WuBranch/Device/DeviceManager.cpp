// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/DeviceManager.h"
#include <WuBranch/Device/KeyboardDevice.h>
#include <WuBranch/Device/CustomDevice.h>
#include <Kismet/KismetSystemLibrary.h>
#include <WuBranch/Device/WiredDevice.h>

UDeviceManager::UDeviceManager()
{
	Devices.Empty();
	SingleDevice = nullptr;
}

void UDeviceManager::CreateAllDevices()
{
	// 手の部位
	// 今は自作デバイスが対応してないため、キーボードデバイスを使う
	UDevice* HandDevice = CreateKeyBoardDevice();
	AddDevice(EDevicePart::Hand, HandDevice);

	// 足の部位
	// プラットフォームごと違うデバイスを使う
	// Android/Ios:無線デバイス、Windows:有線デバイス、キーボードデバイス
#if PLATFORM_WINDOWS
	if (UDevice* WiredDevice = CreateWiredDevice())
	{
		AddDevice(EDevicePart::Foot, WiredDevice);
	}
	else
	{
		// 有線デバイスが繋がらなかったら、キーボードデバイスを使う
		UDevice* KeyboardDevice = CreateKeyBoardDevice();
		AddDevice(EDevicePart::Foot, KeyboardDevice);
	}
#elif PLATFORM_ANDROID || PLATFORM_IOS
	if (UDevice* BleDevice = CreateBleDevice())
	{
		AddDevice(EDevicePart::Foot, BleDevice);
	}
#endif
}

void UDeviceManager::DisConnectAllDevices()
{
	TArray<EDevicePart> AllDeviceParts;
	Devices.GenerateKeyArray(AllDeviceParts);
	for (EDevicePart Part : AllDeviceParts)
	{
		UDevice* Device = GetDevice(Part);
		if (Device->Disconnect())
		{
			DeleteDevice(Part);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Remove %s device fail"), *UEnum::GetValueAsString(Part));
		}
	}
}

void UDeviceManager::EnableAllDevices()
{
	TArray<EDevicePart> AllDeviceParts;
	Devices.GenerateKeyArray(AllDeviceParts);
	for (EDevicePart Part : AllDeviceParts)
	{
		UDevice* Device = GetDevice(Part);
		Device->Enable();
	}
}

void UDeviceManager::AddDevice(EDevicePart Part, UDevice* Device)
{
	if (!Devices.Contains(Part))
	{
		Devices.Add(Part, Device);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("This device already exists, so this operation will be ignored. If you want to replace the device, please delete it first and then add a new one."));
	}
}

void UDeviceManager::DeleteDevice(EDevicePart Part)
{
	if (!Devices.Contains(Part))
	{
		UE_LOG(LogTemp, Warning, TEXT("This device cannot be found, so this operation will be ignored."));
	}
	else
	{
		Devices.Remove(Part);
	}
}

UDevice* UDeviceManager::GetDevice(EDevicePart Part)
{
	if (Devices.IsEmpty())
		return nullptr;

	UDevice* Device = *Devices.Find(Part);
	if (Device)
		return Device;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Device not found!"));
		return nullptr;
	}
}

void UDeviceManager::ChangeDevice(EDeviceType Type)
{
	switch (Type)
	{
	case EDeviceType::UESupportDevice:
		SingleDevice = CreateKeyBoardDevice();
		break;
	case EDeviceType::CustomDevice:
#if PLATFORM_WINDOWS
		SingleDevice = CreateWiredDevice();
#elif PLATFORM_ANDROID || PLATFORM_IOS
		SingleDevice = CreateBleDevice();
#endif
		break;
	default:
		FString TypeName = UEnum::GetDisplayValueAsText(Type).ToString();
		UE_LOG(LogTemplateDevice, Error, TEXT("The constructor method for this device type (%s) has not been implemented yet."), *TypeName);
		break;
	}
}

UDevice* UDeviceManager::GetDevice()
{
	return SingleDevice;
}

void UDeviceManager::EnableDefaultActions()
{
	// 一つの装置のみの場合
	//IMoveProvider::Execute_EnableMoveAction(Device);

	// 複数の装置の場合
	UDevice* Device = GetDevice(EDevicePart::Foot);
	if(Device && Device->Implements<UMoveProvider>())
		IMoveProvider::Execute_EnableMoveAction(Device);
}

void UDeviceManager::DisableDefaultActions()
{
	// 一つの装置のみの場合
	//IMoveProvider::Execute_DisableMoveAction(Device);

	// 複数の装置の場合
	UDevice* Device = GetDevice(EDevicePart::Foot);
	if (Device && Device->Implements<UMoveProvider>())
		IMoveProvider::Execute_DisableMoveAction(Device);
}

void UDeviceManager::EnableSelectAnswerActions()
{
	// 一つの装置のみの場合
	//IChoiceProvider::Execute_EnableSelectAnswerAction(Device);

	// 複数の装置の場合
	UDevice* Device = GetDevice(EDevicePart::Hand);
	if (Device && Device->Implements<UChoiceProvider>())
		IChoiceProvider::Execute_EnableSelectAnswerAction(Device);
}

void UDeviceManager::DisableSelectAnswerActions()
{
	// 一つの装置のみの場合
	//IChoiceProvider::Execute_DisableSelectAnswerAction(Device);

	// 複数の装置の場合
	UDevice* Device = GetDevice(EDevicePart::Hand);
	if (Device && Device->Implements<UChoiceProvider>())
		IChoiceProvider::Execute_DisableSelectAnswerAction(Device);
}

void UDeviceManager::EnableTakePhotoActions()
{
	// 一つの装置のみの場合
	//IChoiceProvider::Execute_EnableSelectAnswerAction(Device);

	// 複数の装置の場合
	UDevice* Device = GetDevice(EDevicePart::Hand);
	if (Device && Device->Implements<UPhotoProvider>())
		IPhotoProvider::Execute_EnableTakePhotoAction(Device);
}

void UDeviceManager::DisableTakePhotoActions()
{
	// 一つの装置のみの場合
	//IChoiceProvider::Execute_DisableSelectAnswerAction(Device);

	// 複数の装置の場合
	UDevice* Device = GetDevice(EDevicePart::Hand);
	if (Device && Device->Implements<UPhotoProvider>())
		IPhotoProvider::Execute_DisableTakePhotoAction(Device);
}

void UDeviceManager::BindMoveEvent(UObject* Object, FName FunctionName)
{
	// 一つの装置のみの場合
	//IMoveProvider::Execute_BindMoveEvent(Device, Object, FunctionName);

	// 複数の装置の場合
	UDevice* Device = GetDevice(EDevicePart::Foot);
	if (Device && Device->Implements<UMoveProvider>())
	{
		IMoveProvider::Execute_BindMoveEvent(Device, Object, FunctionName);
	}
}

void UDeviceManager::BindMoveNumEvent(UObject* Object, FName FunctionName)
{
	UDevice* Device = GetDevice(EDevicePart::Foot);
	if (Device && Device->Implements<UMoveProvider>())
	{
		IMoveProvider::Execute_BindMoveNumEvent(Device, Object, FunctionName);
	}
}

void UDeviceManager::BindSelectLeftEvent(UObject* Object, FName FunctionName)
{
	// 一つの装置のみの場合
	//IChoiceProvider::Execute_BindSelectLeftEvent(Device, Object, FunctionName);

	// 複数の装置の場合
	UDevice* Device = GetDevice(EDevicePart::Hand);
	if (Device && Device->Implements<UChoiceProvider>())
	{
		IChoiceProvider::Execute_BindSelectLeftEvent(Device, Object, FunctionName);
	}
}

void UDeviceManager::BindSelectRightEvent(UObject* Object, FName FunctionName)
{
	// 一つの装置のみの場合
	//IChoiceProvider::Execute_BindSelectRightEvent(Device, Object, FunctionName);

	// 複数の装置の場合
	UDevice* Device = GetDevice(EDevicePart::Hand);
	if (Device && Device->Implements<UChoiceProvider>())
	{
		IChoiceProvider::Execute_BindSelectRightEvent(Device, Object, FunctionName);
	}
}

void UDeviceManager::BindScreenshotEvent(UObject* Object, FName FunctionName)
{
	if (!Object)
	{
		UE_LOG(LogTemp, Error, TEXT("BindScreenshotEvent: Object is null!"));
		return;
	}

	// Handデバイスのみにバインド
	UDevice* Device = GetDevice(EDevicePart::Hand);
	if (Device && Device->Implements<UPhotoProvider>())
	{
		IPhotoProvider::Execute_BindTakePhotoEvent(Device, Object, FunctionName);
		UE_LOG(LogTemp, Log, TEXT("Screenshot event bound to %s::%s"), *Object->GetName(), *FunctionName.ToString());
	}

	/*if (Device)
	{
		if (UKeyboardDevice* KeyboardDevice = Cast<UKeyboardDevice>(Device))
		{
			FScriptDelegate Delegate;
			Delegate.BindUFunction(Object, FuncName);
			KeyboardDevice->OnScreenshotEvent.Add(Delegate);
			
			UE_LOG(LogTemp, Log, TEXT("Screenshot event bound to %s::%s"), *Object->GetName(), *FuncName.ToString());
		}
	}*/
}

void UDeviceManager::BindPhotoReadyEvent(UObject* Object, FName FunctionName)
{
	if (!Object)
	{
		UE_LOG(LogTemp, Error, TEXT("BindPhotoReadyEvent: Object is null!"));
		return;
	}

	// Handデバイスのみにバインド
	UDevice* Device = GetDevice(EDevicePart::Hand);
	if (Device && Device->Implements<UPhotoProvider>())
	{
		IPhotoProvider::Execute_BindPhotoReadyEvent(Device, Object, FunctionName);
		UE_LOG(LogTemp, Log, TEXT("PhotoReady event bound to %s::%s"), *Object->GetName(), *FunctionName.ToString());
	}
}

UDevice* UDeviceManager::CreateKeyBoardDevice()
{
	UDevice* Device = NewObject<UKeyboardDevice>(this);
	Device->Init();
	Device->Connect();
	return Device;
}

UDevice* UDeviceManager::CreateWiredDevice()
{
	UDevice * Device = NewObject<UWiredDevice>(this);
	Device->Init(WiredDeviceID, WiredDeviceVer);
	bool bResult = Device->Connect();
	if (bResult)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Wired Device Connect Success"));
		return Device;
	}
	return nullptr;
}

UDevice* UDeviceManager::CreateBleDevice()
{
	UDevice* Device = NewObject<UCustomDevice>(this);
	if (Device)
	{
		Device->Init();
		return Device;
	}
	return nullptr;
}
