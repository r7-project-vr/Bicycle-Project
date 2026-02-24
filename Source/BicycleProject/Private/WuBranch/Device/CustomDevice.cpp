// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/CustomDevice.h"
#include <WuBranch/Device/DeviceType.h>
#include <WuBranch/Struct/BLEDeviceInfo.h>
#if PLATFORM_ANDROID
#include "BleUtils.h"
#include "Interface/BleManagerInterface.h"
#include "Interface/BleDeviceInterface.h"
#include "AndroidPermissionFunctionLibrary.h"
#include "AndroidPermissionCallbackProxy.h"
#include <WuBranch/MyGameInstance.h>
#endif

UCustomDevice::UCustomDevice()
	: BleManager(nullptr)
	, CurrentDevice(nullptr)
	, LEDCheckTimer(0.0f)
	, bIsMakeList(false)
	, bMoveSwitch(false)
{
}

UCustomDevice::~UCustomDevice()
{
#if PLATFORM_ANDROID
	if (CurrentDevice)
	{
		if (State == EDeviceConnectType::Connected)
		{
			Disconnect();
		}
	}

#endif
}

void UCustomDevice::Init()
{
#if !PLATFORM_ANDROID && !PLATFORM_IOS
	UE_LOG(LogTemplateDevice, Error, TEXT("Sorry, this class only support for android or ios platform because of the plugin."));
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("Sorry, this class only support for android or ios platform because of the plugin."));
#endif

#if PLATFORM_ANDROID
	BleManager = UBleUtils::CreateBleManager();
	
	if (!CheckBluetooth())
		return;

	// この以降はデバイスのbluetoothがオンの状態かつBluetooth Low Energy(BLE)がサポートしている状態
	// 初期化
	bIsMakeList = false;
	LEDCheckTimer = 0.0f;
	DevicesWaiting.Empty();
	ResetDeviceList();
	// サービスからデバイスを見つける
	DecideTargetServices();
	// 権限を要求する
	RequestAndroidPermission();
	FindDeviceByServices();

	UMyGameInstance* GameInstance = GetWorld()->GetGameInstance<UMyGameInstance>();
	GameInstance->OnUpdateRPM.AddDynamic(this, &UCustomDevice::UpdateMaxRPM);
	MaxRPM = GameInstance->GetDangerRPM();
#endif
}

void UCustomDevice::Enable()
{
}

bool UCustomDevice::Connect()
{
#if PLATFORM_ANDROID
	FBleDelegate SuccFunction;
	SuccFunction.BindUFunction(this, FName("OnConnectSucc"));
	FBleErrorDelegate ErrFunction;
	ErrFunction.BindUFunction(this, FName("OnConnectError"));
	CurrentDevice.GetInterface()->Connect(SuccFunction, ErrFunction);
	State = EDeviceConnectType::Connecting;
#endif
	return true;
}

bool UCustomDevice::Disconnect()
{
#if PLATFORM_ANDROID
	FBleDelegate SuccFunction;
	SuccFunction.BindUFunction(this, FName("OnDisconnectSucc"));
	FBleErrorDelegate ErrFunction;
	ErrFunction.BindUFunction(this, FName("OnDisconnectError"));
	CurrentDevice.GetInterface()->Disconnect(SuccFunction, ErrFunction);
	State = EDeviceConnectType::Disconnecting;
#endif
	return true;
}

void UCustomDevice::EnableMoveAction_Implementation()
{
	bMoveSwitch = true;
}

void UCustomDevice::DisableMoveAction_Implementation()
{
	bMoveSwitch = false;
}

void UCustomDevice::EnableSelectAnswerAction_Implementation()
{
}

void UCustomDevice::DisableSelectAnswerAction_Implementation()
{
}

void UCustomDevice::Tick(float DeltaTime)
{
	return;
#if PLATFORM_ANDROID
	// 0.5秒ごとにDevicesWaitingをみて、LEDを確認する必要があるデバイスがあると確認する
	/*if (DevicesWaiting.Num() > 0)
	{
		if (LEDCheckTimer <= 0.0f)
		{
			if(!CurrentDevice)
				CheckDeviceLED();
		}
		else
		{
			LEDCheckTimer -= DeltaTime;
		}
	}*/
#endif
}

TStatId UCustomDevice::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCustomDevice, STATGROUP_Tickables);
}

bool UCustomDevice::IsTickableInEditor() const
{
	return false;
}

bool UCustomDevice::CheckBluetooth()
{
#if PLATFORM_ANDROID
	if (BleManager)
	{
		if (!BleManager.GetInterface()->IsBleSupported())
		{
			UE_LOG(LogTemplateDevice, Error, TEXT("This device is not support Bluetooth low energy"));
			return false;
		}

		if (!BleManager.GetInterface()->IsBluetoothEnabled())
		{
			UE_LOG(LogTemplateDevice, Warning, TEXT("This device did not open bluetooth"));
			UE_LOG(LogTemplateDevice, Warning, TEXT("Open bluetooth"));
			BleManager.GetInterface()->SetBluetoothState(true);
		}
		return true;
	}
#endif

	return false;
}

void UCustomDevice::CheckDeviceLED()
{
	FBLEDeviceInfo Target = DevicesWaiting[0];
	CurrentDevice = Target.Device;
	Connect();
}

void UCustomDevice::ResetDeviceList()
{
	DeviceList.Empty();
}

FBLEDeviceInfo UCustomDevice::MakeDeviceBaseInfo(TScriptInterface<IBleDeviceInterface> Device, FString DeviceName, FString DeviceUUID)
{
	FBLEDeviceInfo Info;
	Info.Name = DeviceName;
	Info.UUID = DeviceUUID;
	Info.State = EDeviceConnectType::UnConnected;
	Info.LEDColor1 = FColor::Black;
	Info.LEDColor2 = FColor::Black;
	return Info;
}

void UCustomDevice::AddToDeviceList(FBLEDeviceInfo Device)
{
	DeviceList.Add(Device);
}

void UCustomDevice::RequestAndroidPermission()
{
#if PLATFORM_ANDROID
	if (!UAndroidPermissionFunctionLibrary::CheckPermission(ANDROID_FILE_LOCATION_PERMISSION))
	{
		TArray<FString> Permissions;
		Permissions.Add(ANDROID_FILE_LOCATION_PERMISSION);
		Permissions.Add(ANDROID_BLUETOOTH_CONNECT_PERMISSION);
		Permissions.Add(ANDROID_BLUETOOTH_SCAN_PERMISSION);
		UAndroidPermissionCallbackProxy* Callback = UAndroidPermissionFunctionLibrary::AcquirePermissions(Permissions);
		Callback->OnPermissionsGrantedDynamicDelegate.AddDynamic(this, &UCustomDevice::OnPermissionResult);
	}
#endif
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
	// UUIDで先にデバイスを区別したいが、なぜか何一つデバイスを検知できない
	//Services.Add(IO_SERVICE_UUID);
}

void UCustomDevice::FindDeviceByServices()
{
#if PLATFORM_ANDROID
	if (BleManager)
	{
		FBleOnDeviceFoundDelegate Function;
		Function.BindUFunction(this, FName("OnDeviceFound"));
		BleManager.GetInterface()->ScanForDevices(Services, Function);
	}
#endif
}

void UCustomDevice::OnDeviceFound(TScriptInterface<IBleDeviceInterface> Device)
{
#if PLATFORM_ANDROID
	if (Device)
	{
		// デバイスの名前で接続したいデバイスかどうかを判別する
		if (!Device.GetInterface()->GetDeviceName().Equals(IO_DEVICE_NAME))
			return;

		// 新しく見つけたデバイスを使用する
		// 既に接続した場合は切断する
		if (CurrentDevice && State == EDeviceConnectType::Connected)
		{
			Disconnect();
		}
		else if ((!CurrentDevice && State == EDeviceConnectType::Connecting) || (CurrentDevice && State == EDeviceConnectType::Disconnecting))
		{
			// 接続しているか切断しているか
			// 何もしない、やっていることが終了するまで待つ
		}
		else
		{
			UE_LOG(LogTemplateDevice, Display, TEXT("Connect to Device: %s"), *Device.GetInterface()->GetDeviceName());

			CurrentDevice = Device;

			Connect();

			// つなげそうなデバイス見つけたら、DevicesWaitingに入れてLEDの色を確認する
			//FString DeviceName = Device.GetInterface()->GetDeviceName();
			//FString DeviceUUID = Device.GetInterface()->GetDeviceId();
			//FBLEDeviceInfo PendingDevice = MakeDeviceBaseInfo(Device, DeviceName, DeviceUUID);
			//DevicesWaiting.Add(PendingDevice);

			//　スキャンを止める
			BleManager.GetInterface()->StopScan();
		}
	}
#endif
}

void UCustomDevice::OnConnectSucc()
{
#if PLATFORM_ANDROID
	UE_LOG(LogTemplateDevice, Display, TEXT("Connect to device successfully"));
	Name = CurrentDevice.GetInterface()->GetDeviceName();
	UUID = CurrentDevice.GetInterface()->GetDeviceId();
	State = EDeviceConnectType::Connected;
	// まずどの段階にいるかを確認
	if (bIsMakeList)
	{
		GetValidationCode();
	}
	else
	{
		// つなぐデバイスが決めた
		SendPairRequest();
	}
#endif
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
	CurrentDevice = nullptr;
	LEDCheckTimer = LEDCheckTimeDuration;
}

void UCustomDevice::OnDisconnectError(FString ErrorMessage)
{
	UE_LOG(LogTemplateDevice, Error, TEXT("Disconnect to device failed: %s"), *ErrorMessage);
	State = EDeviceConnectType::Connected;
}

void UCustomDevice::GetValidationCode()
{
#if PLATFORM_ANDROID
	UE_LOG(LogTemplateDevice, Display, TEXT("Do Get Validation Code"));
	FBleCharacteristicDelegate ReceiveFunction;
	ReceiveFunction.BindUFunction(this, FName("OnReceiveData"));
	CurrentDevice.GetInterface()->BindToCharacteristicWriteEvent(ReceiveFunction);
	CurrentDevice.GetInterface()->ReadCharacteristic(IO_PAIR_SERVICE_UUID, IO_LED_COLOR_CHARACTERISTIC_UUID);
	UE_LOG(LogTemplateDevice, Error, TEXT("Write to : %s, %s"), IO_PAIR_SERVICE_UUID, IO_LED_COLOR_CHARACTERISTIC_UUID);
#endif
}

void UCustomDevice::SendPairRequest()
{
#if PLATFORM_ANDROID
	UE_LOG(LogTemplateDevice, Display, TEXT("Do Write pair"));
	FBleCharacteristicDelegate WriteFunction;
	WriteFunction.BindUFunction(this, FName("OnWriteData"));
	CurrentDevice.GetInterface()->BindToCharacteristicWriteEvent(WriteFunction);
	TArray<uint8> Datas;
	Datas.Add(1);
	CurrentDevice.GetInterface()->WriteCharacteristic(IO_PAIR_SERVICE_UUID, IO_PAIR_CHARACTERISTIC_UUID, Datas);
#endif
}

template<typename T>
T UCustomDevice::TransformDataToInt(const uint8_t* Data, int Size) const
{
	// 単位：Byte
	//RPMのデータは2バイト, Data[0]が上位バイト, Data[1]が下位バイト
	T Result = 0;
	for (int Index = 0; Index < Size; ++Index)
	{
		Result |= (Data[Index] << (8 * (Size - 1 - Index)));
	}
	return Result;
}

void UCustomDevice::OnWriteData(FString ServiceUUID, FString CharacteristicUUID)
{
#if PLATFORM_ANDROID
	FBleCharacteristicDataDelegate ReceiveFunction;
	ReceiveFunction.BindUFunction(this, FName("OnReceiveData"));
	CurrentDevice.GetInterface()->BindToCharacteristicNotificationEvent(ReceiveFunction);
	CurrentDevice.GetInterface()->SubscribeToCharacteristic(IO_BIKE_SERVICE_UUID, IO_RPM_CHARACTERISTIC_UUID, false);
	CurrentDevice.GetInterface()->SubscribeToCharacteristic(IO_BIKE_SERVICE_UUID, IO_REVOLUTION_CHARACTERISTIC_UUID, false);
	UE_LOG(LogTemplateDevice, Error, TEXT("Write to : %s, %s"), *ServiceUUID, *CharacteristicUUID);
#endif
}

void UCustomDevice::OnReceiveData(FString ServiceUUID, FString CharacteristicUUID, TArray<uint8>& Data)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(TEXT("Receive %s data"), *CharacteristicUUID));
	if (ServiceUUID.Equals(IO_BIKE_SERVICE_UUID) && CharacteristicUUID.Equals(IO_RPM_CHARACTERISTIC_UUID))
		HandleRPMData(Data);
	else if (ServiceUUID.Equals(IO_BIKE_SERVICE_UUID) && CharacteristicUUID.Equals(IO_RPS_CHARACTERISTIC_UUID))
		HandleRPSData(Data);
	else if (ServiceUUID.Equals(IO_BIKE_SERVICE_UUID) && CharacteristicUUID.Equals(IO_REVOLUTION_CHARACTERISTIC_UUID))
		HandleRevolutionData(Data);
	else if (ServiceUUID.Equals(IO_PAIR_SERVICE_UUID) && CharacteristicUUID.Equals(IO_LED_COLOR_CHARACTERISTIC_UUID))
		HandleLEDData(Data);
}

void UCustomDevice::HandleRPMData(const TArray<uint8>& Data)
{
	//DebugReceiveData(Data);
	// 無線だと直接数字がもらえる、変換しなくていい
	//uint16 RPM = TransformDataToInt<uint16>(Data.GetData(), Data.Num());
	uint16 RPM = Data[0];
	float InputVelocity = FMath::Clamp(RPM / MaxRPM, 0.f, 1.f);
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("RPM: %d, Velocity: %lf"), RPM, InputVelocity));
	FVector2D MoveVector(InputVelocity, 0);
	NotifyMoveEvent(MoveVector);
}

void UCustomDevice::HandleRPSData(const TArray<uint8>& Data)
{
	uint8 Battery = TransformDataToInt<uint8>(Data.GetData(), Data.Num());
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Battery: %d%%"), Battery));
}

void UCustomDevice::HandleRevolutionData(const TArray<uint8>& Data)
{
	uint32 Revolution = TransformDataToInt<uint32>(Data.GetData(), Data.Num());
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(TEXT("Revolution: %d"), Revolution));
}

void UCustomDevice::HandleLEDData(const TArray<uint8>& Data)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("Get Data Length: %d"), Data.Num()));
	FBLEDeviceInfo Target = DevicesWaiting[0];
	uint32 LED[2];
	LED[0] = TransformDataToInt<uint32>(Data.GetData(), 4);
	LED[1] = TransformDataToInt<uint32>(&Data[4], 4);
	Target.LEDColor1 = FColor(LED[0]);
	Target.LEDColor2 = FColor(LED[1]);
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("LED 1: %u, LED 2 : %u"), LED[0], LED[1]));
	DeviceList.Add(Target);
	DevicesWaiting.Remove(Target);
	Disconnect();
}

void UCustomDevice::DebugReceiveData(const TArray<uint8>& Data)
{
	FString Content;
	for (uint8 D : Data)
		Content += FString::FromInt(D) + TEXT(", ");
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Content: %s, Num* %d"), *Content, Data.Num()));
}

void UCustomDevice::NotifyDeviceListChangedEvent()
{
	if (OnDeviceListChanged.IsBound())
		OnDeviceListChanged.Broadcast(DeviceList);
}

void UCustomDevice::NotifyMoveEvent(FVector2D MoveData)
{
	if(!bMoveSwitch)
		return;

	// 通知する
	if (OnMoveEvent.IsBound())
		OnMoveEvent.Broadcast(MoveData);
}

void UCustomDevice::UpdateMaxRPM(int Standard, int Danger, int Safe)
{
	// 危険値を最大値として使う
	MaxRPM = Danger;
}