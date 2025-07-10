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
	: DefaultActionSwitch(false)
	, SelectAnswerSwitch(false)
	, BleManager(nullptr)
	, MyDevice(nullptr)
{
}

void UCustomDevice::Init()
{
#if !PLATFORM_ANDROID || !PLATFORM_IOS
	UE_LOG(LogTemplateDevice, Error, TEXT("Sorry, this class only support for android or ios platform because of the plugin."));
	return;
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

	EnableDefaultActions_Implementation();
//#endif
}

void UCustomDevice::EnableDefaultActions_Implementation()
{
	DefaultActionSwitch = true;
}

void UCustomDevice::DisableDefaultActions_Implementation()
{
	DefaultActionSwitch = false;
}

void UCustomDevice::EnableSelectAnswerActions_Implementation()
{
	SelectAnswerSwitch = true;
}

void UCustomDevice::DisableSelectAnswerActions_Implementation()
{
	SelectAnswerSwitch = false;
}

void UCustomDevice::Connect_Implementation()
{
//#if PLATFORM_ANDROID
	FBleDelegate SuccFunction;
	SuccFunction.BindUFunction(this, FName("OnConnectSucc"));
	FBleErrorDelegate ErrFunction;
	ErrFunction.BindUFunction(this, FName("OnConnectError"));
	MyDevice->Connect(SuccFunction, ErrFunction);
	_state = EDeviceConnectType::Connecting;
//#endif
}

void UCustomDevice::Disconnect_Implementation()
{
//#if PLATFORM_ANDROID
	FBleDelegate SuccFunction;
	SuccFunction.BindUFunction(this, FName("OnDisconnectSucc"));
	FBleErrorDelegate ErrFunction;
	ErrFunction.BindUFunction(this, FName("OnDisconnectError"));
	MyDevice->Disconnect(SuccFunction, ErrFunction);
	_state = EDeviceConnectType::Disconnecting;
//#endif
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
		if (_state == EDeviceConnectType::Connected)
		{
			Disconnect_Implementation();
		}
		else if (_state == EDeviceConnectType::Connecting || _state == EDeviceConnectType::Disconnecting)
		{
			// 接続しているか切断しているか
			// 何もしない、やっていることが終了するまで待つ
		}
		else
		{
			MyDevice = DeviceInterface;
			
			// 接続する
			Connect_Implementation();
		}
	}
//#endif
}

void UCustomDevice::OnConnectSucc()
{
	UE_LOG(LogTemplateDevice, Display, TEXT("Connect to device successfully"));
	_name = MyDevice->GetDeviceName();
	_uuid = MyDevice->GetDeviceId();
	_state = EDeviceConnectType::Connected;
}

void UCustomDevice::OnConnectError(FString ErrorMessage)
{
	UE_LOG(LogTemplateDevice, Error, TEXT("Connect to device failed: %s"), *ErrorMessage);
	_state = EDeviceConnectType::Disconnected;
}

void UCustomDevice::OnDisconnectSucc()
{
	UE_LOG(LogTemplateDevice, Display, TEXT("Disconnect to device successfully"));
	_name.Empty();
	_uuid.Empty();
	_state = EDeviceConnectType::Disconnected;
}

void UCustomDevice::OnDisconnectError(FString ErrorMessage)
{
	UE_LOG(LogTemplateDevice, Error, TEXT("Disconnect to device failed: %s"), *ErrorMessage);
	_state = EDeviceConnectType::Connected;
}

void UCustomDevice::OnMove()
{
	// (Y, X)
	// Y: 前後, X: 左右(無視)
	FVector2D MoveVector(0, 0);

	if (DefaultActionSwitch && _onMoveEvent.IsBound())
		_onMoveEvent.Broadcast(MoveVector);
}

void UCustomDevice::OnSelectLeftAnswer()
{
	if (SelectAnswerSwitch && _onSelectLeftEvent.IsBound())
		_onSelectLeftEvent.Broadcast();
}

void UCustomDevice::OnSelectRightAnswer()
{
	if (SelectAnswerSwitch && _onSelectRightEvent.IsBound())
		_onSelectRightEvent.Broadcast();
}
