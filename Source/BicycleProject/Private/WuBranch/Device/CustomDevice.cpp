// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/CustomDevice.h"
#include "BleUtils.h"
#include <WuBranch/Device/DeviceType.h>
//#if PLATFORM_ANDROID
#include "Interface/BleManagerInterface.h"
#include "Interface/BleDeviceInterface.h"
#include "AndroidPermissionFunctionLibrary.h"
#include "AndroidPermissionCallbackProxy.h"
//#include "../../../../../../../../../../Program Files/Epic Games/UE_5.4/Engine/Plugins/Runtime/AndroidPermission/Source/AndroidPermission/Classes/AndroidPermissionFunctionLibrary.h"
//#endif

UCustomDevice::UCustomDevice()
	: MoveSwitch(false)
	, BleManager(nullptr)
	, MyDevice(nullptr)
{
}

void UCustomDevice::Init()
{
#if !PLATFORM_ANDROID || !PLATFORM_IOS
	UE_LOG(LogTemplateDevice, Error, TEXT("Sorry, this class only support for android or ios platform because of the plugin."));
	//return;
#endif

//#if PLATFORM_ANDROID
	BleManager = Cast<IBleManagerInterface>(UBleUtils::CreateBleManager().GetObject());

	if (!CheckBluetooth())
		return;

	// この以降はデバイスのbluetoothがオンの状態かつBluetooth Low Energy(BLE)がサポートしている状態
	
	// サービスからデバイスを見つける
	DecideTargetServices();
	// 権限を要求する
	RequestAndroidPermission();
	FindDeviceByServices();

	EnableMoveAction_Implementation();
//#endif
}

bool UCustomDevice::Connect()
{
//#if PLATFORM_ANDROID
	FBleDelegate SuccFunction;
	SuccFunction.BindUFunction(this, FName("OnConnectSucc"));
	FBleErrorDelegate ErrFunction;
	ErrFunction.BindUFunction(this, FName("OnConnectError"));
	MyDevice->Connect(SuccFunction, ErrFunction);
	State = EDeviceConnectType::Connecting;
//#endif
	return true;
}

bool UCustomDevice::Disconnect()
{
//#if PLATFORM_ANDROID
	FBleDelegate SuccFunction;
	SuccFunction.BindUFunction(this, FName("OnDisconnectSucc"));
	FBleErrorDelegate ErrFunction;
	ErrFunction.BindUFunction(this, FName("OnDisconnectError"));
	MyDevice->Disconnect(SuccFunction, ErrFunction);
	State = EDeviceConnectType::Disconnecting;
//#endif
	return true;
}

void UCustomDevice::EnableMoveAction_Implementation()
{
	MoveSwitch = true;
}

void UCustomDevice::DisableMoveAction_Implementation()
{
	MoveSwitch = false;
}

void UCustomDevice::EnableSelectAnswerAction_Implementation()
{
}

void UCustomDevice::DisableSelectAnswerAction_Implementation()
{
}

bool UCustomDevice::CheckBluetooth()
{
//#if PLATFORM_ANDROID
	if (BleManager)
	{
		if (!BleManager->IsBleSupported())
		{
			UE_LOG(LogTemplateDevice, Error, TEXT("This device is not support Bluetooth low energy"));
			return false;
		}

		if (!BleManager->IsBluetoothEnabled())
		{
			UE_LOG(LogTemplateDevice, Warning, TEXT("This device did not open bluetooth"));
			UE_LOG(LogTemplateDevice, Warning, TEXT("Open bluetooth"));
			BleManager->SetBluetoothState(true);
			return true;
		}
	}
//#endif
	return false;
}

void UCustomDevice::RequestAndroidPermission()
{
//#if PLATFORM_ANDROID
	if (!UAndroidPermissionFunctionLibrary::CheckPermission(ANDROID_FILE_LOCATION_PERMISSION))
	{
		TArray<FString> Permissions;
		Permissions.Add(ANDROID_FILE_LOCATION_PERMISSION);
		Permissions.Add(ANDROID_BLUETOOTH_CONNECT_PERMISSION);
		Permissions.Add(ANDROID_BLUETOOTH_SCAN_PERMISSION);
		UAndroidPermissionCallbackProxy* Callback = UAndroidPermissionFunctionLibrary::AcquirePermissions(Permissions);
		Callback->OnPermissionsGrantedDynamicDelegate.AddDynamic(this, &UCustomDevice::OnPermissionResult);
	}
//#endif
}

void UCustomDevice::OnPermissionResult(const TArray<FString>& Permissions, const TArray<bool>& GrantResults)
{
//#if PLATFORM_ANDROID
	
//#endif
}

void UCustomDevice::DecideTargetServices()
{
	// 少なくとも一つのサービスが必要
	// そうでないと、すべてのデバイスが見つかって戻ってきって、大量のリソースが消耗されるだそうです。
	// https://docs.ninevastudios.com/#/ue-plugins/ble-goodies?id=setup
	UE_LOG(LogTemplateDevice, Display, TEXT("Prepare services's uuid"));
	Services.Empty();
	Services.Add(IO_SERVICE_UUID);
}

void UCustomDevice::FindDeviceByServices()
{
//#if PLATFORM_ANDROID
	if (BleManager)
	{
		FBleOnDeviceFoundDelegate Function;
		Function.BindUFunction(this, FName("OnDeviceFound"));
		FString JoinedString = FString::Join(Services, TEXT(","));
		UE_LOG(LogTemplateDevice, Display, TEXT("Scan for devices: %s"), *JoinedString);
		BleManager->ScanForDevices(Services, Function);
	}
//#endif
}

void UCustomDevice::OnDeviceFound(TScriptInterface<IBleDeviceInterface> Device)
{
//#if PLATFORM_ANDROID
	if (IBleDeviceInterface* DeviceInterface = Cast<IBleDeviceInterface>(Device.GetObject()))
	{
		// 新しく見つけたデバイスを使用する
		// 既に接続した場合は切断する
		if (State == EDeviceConnectType::Connected)
		{
			Disconnect();
		}
		else if (State == EDeviceConnectType::Connecting || State == EDeviceConnectType::Disconnecting)
		{
			// 接続しているか切断しているか
			// 何もしない、やっていることが終了するまで待つ
		}
		else
		{
			MyDevice = DeviceInterface;
			
			// 接続する
			Connect();
		}
	}
//#endif
}

void UCustomDevice::OnConnectSucc()
{
	UE_LOG(LogTemplateDevice, Display, TEXT("Connect to device successfully"));
	Name = MyDevice->GetDeviceName();
	UUID = MyDevice->GetDeviceId();
	State = EDeviceConnectType::Connected;
}

void UCustomDevice::OnConnectError(FString ErrorMessage)
{
	UE_LOG(LogTemplateDevice, Error, TEXT("Connect to device failed: %s"), *ErrorMessage);
	State = EDeviceConnectType::UnConnected;
}

void UCustomDevice::OnDisconnectSucc()
{
	UE_LOG(LogTemplateDevice, Display, TEXT("Disconnect to device successfully"));
	Name.Empty();
	UUID.Empty();
	State = EDeviceConnectType::UnConnected;
}

void UCustomDevice::OnDisconnectError(FString ErrorMessage)
{
	UE_LOG(LogTemplateDevice, Error, TEXT("Disconnect to device failed: %s"), *ErrorMessage);
	State = EDeviceConnectType::Connected;
}

void UCustomDevice::OnMove()
{
	// (Y, X)
	// Y: 前後, X: 左右(無視)
	FVector2D MoveVector(0, 0);
	NotifyMoveEvent(MoveVector);
}

void UCustomDevice::NotifyMoveEvent(FVector2D MoveData)
{
	if(!MoveSwitch)
		return;

	// 通知する
	if (OnMoveEvent.IsBound())
		OnMoveEvent.Broadcast(MoveData);
}
