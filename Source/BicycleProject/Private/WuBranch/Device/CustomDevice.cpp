// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/CustomDevice.h"
#include "BleUtils.h"
#include "Interface/BleManagerInterface.h"
#include "Interface/BleDeviceInterface.h"
#include "AndroidPermissionFunctionLibrary.h"

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

	BleManager = Cast<IBleManagerInterface>(UBleUtils::CreateBleManager().GetObject());

	if (!CheckBluetooth())
		return;

	// この以降はデバイスのbluetoothがオンの状態かつBluetooth Low Energy(BLE)がサポートしている状態
	
	// サービスからデバイスを見つける
	DecideTargetServices();
	FindDeviceByServices();

	EnableDefaultActions_Implementation();
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
	FBleDelegate SuccFunction;
	FBleErrorDelegate ErrFunction;
	//DeviceInterface->Connect(SuccFunction, ErrFunction);
}

bool UCustomDevice::CheckBluetooth()
{
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
#if PLATFORM_ANDROID
			UE_LOG(LogTemplateDevice, Warning, TEXT("Open bluetooth"));
			BleManager->SetBluetoothState(true);
			return true;
#else 
			UE_LOG(LogTemplateDevice, Error, TEXT("This device did not open bluetooth"));
			return false;
#endif
		}
	}
	return false;
}

void UCustomDevice::RequestAndroidPermission()
{
#if PLATFORM_ANDROID
	if (!UAndroidPermissionFunctionLibrary::CheckPermission(ANDROID_FILE_LOCATION_PERMISSION))
	{

	}
#endif
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
	if (BleManager)
	{
		FBleOnDeviceFoundDelegate Function;
		Function.BindUFunction(this, FName("OnDeviceFound"));
		FString JoinedString = FString::Join(Services, TEXT(","));
		UE_LOG(LogTemplateDevice, Display, TEXT("Scan for devices: %s"), JoinedString);
		BleManager->ScanForDevices(Services, Function);
	}
}

void UCustomDevice::OnDeviceFound(TScriptInterface<IBleDeviceInterface> Device)
{
	if (IBleDeviceInterface* DeviceInterface = Cast<IBleDeviceInterface>(Device.GetObject()))
	{
		// 元々ない場合
		if (!MyDevice)
			MyDevice = DeviceInterface;
		// 二つ以上見つかった場合
		else if (MyDevice != DeviceInterface) {

		}

		// 繋いでない場合
		if (!DeviceInterface->IsConnected())
		{
			
		}
		else {
			
		}
	}
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
