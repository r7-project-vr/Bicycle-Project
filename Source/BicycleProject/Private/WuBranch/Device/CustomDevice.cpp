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
	, CurrentDeviceInfo(nullptr)
	, LEDCheckTimer(0.0f)
	, bIsMakeList(false)
	, bMoveSwitch(false)
	, bIsOperationInProgress(false)
	, bNeedReScan(false)
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
	bNeedReScan = false;
	LEDCheckTimer = 0.0f;
	DevicesWaiting.Empty();
	ResetDeviceList();
	ClearOperationQueue();
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
	if (!CurrentDevice)
		return false;

#if PLATFORM_ANDROID
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, FString::Printf(TEXT("Start Connect")));
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
	if (!CurrentDevice)
		return false;

#if PLATFORM_ANDROID
	// 切断時はQueueをクリアして中途半端な操作が残らないようにする
	ClearOperationQueue();

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

void UCustomDevice::ResetRevolution()
{
#if PLATFORM_ANDROID
	TArray<uint8> Datas;
	EnqueueOperation(FBleOperation::MakeWrite(IO_BIKE_SERVICE_UUID, IO_REVOLUTION_RESET_CHARACTERISTIC_UUID, Datas));
#endif
}

void UCustomDevice::Tick(float DeltaTime)
{
	return;
#if PLATFORM_ANDROID
	// 0.5秒ごとにDevicesWaitingをみて、LEDを確認する必要があるデバイスがあると確認する
	if (DevicesWaiting.Num() > 0)
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
	}
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

TArray<FBLEDeviceInfo> UCustomDevice::GetDeviceList() const
{
	return DeviceList;
}

void UCustomDevice::ConnectByUUID(const FString& ID)
{
	CurrentDeviceInfo = DeviceList.FindByPredicate([&](const FBLEDeviceInfo& Info) { return Info == ID; });

	if(!CurrentDeviceInfo)
	{
		UE_LOG(LogTemplateDevice, Warning, TEXT("Didnot find target device in the list. UUID: %s"), *ID);
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("Didnot find target device in the list. UUID: %s"), *ID));
		return;
	}
	
	// 既に接続している場合は切断する(複数デバイスがある状況)
	/*if (CurrentDevice && State == EDeviceConnectType::Connected)
	{
		Disconnect();
	}*/

#if PLATFORM_ANDROID
	//　スキャンを止める(複数デバイスがある状況)
	//BleManager.GetInterface()->StopScan();
	//bIsMakeList = false;
	//CurrentDevice = CurrentDeviceInfo->Device;
	
	// つなぐデバイスが決めた
	SendPairRequest(1);
	CurrentDeviceInfo->ChangeState(EDeviceConnectType::Connected);
	NotifyDeviceInfoChangedEvent();
#endif
}

void UCustomDevice::DisconnectByUUID(const FString& ID)
{
	if (CurrentDeviceInfo->UUID != ID)
	{
		UE_LOG(LogTemplateDevice, Warning, TEXT("The target device is not the current connected device. UUID: %s"), *ID);
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("The target device is not the current connected device. UUID: %s"), *ID));
		return;
	}

#if PLATFORM_ANDROID
	// つなぐデバイスが決めた
	SendPairRequest(0);
	CurrentDeviceInfo->ChangeState(EDeviceConnectType::UnConnected);
	NotifyDeviceInfoChangedEvent();
#endif
}

void UCustomDevice::StopScanDevices()
{
#if PLATFORM_ANDROID
	if (BleManager)
	{
		BleManager.GetInterface()->StopScan();
	}
#endif
}

void UCustomDevice::ReScanDevices()
{
#if PLATFORM_ANDROID
	bNeedReScan = true;
	if (!Disconnect())
	{
		OnDisconnectSucc();
	}
#endif
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
	// (デバイスが１つのみの場合)
	CurrentDeviceInfo = &DevicesWaiting[0];
	if(CurrentDeviceInfo)
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, FString::Printf(TEXT("Has Device Info")));
	CurrentDevice = CurrentDeviceInfo->Device;
	if(CurrentDevice)
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, FString::Printf(TEXT("Has Device")));
	Connect();
}

void UCustomDevice::ResetDeviceList()
{
	DeviceList.Empty();
}

FBLEDeviceInfo UCustomDevice::MakeDeviceBaseInfo(TScriptInterface<IBleDeviceInterface> Device, FString DeviceName, FString DeviceUUID)
{
	FBLEDeviceInfo Info;
	Info.Device = Device;
	Info.Name = DeviceName;
	Info.UUID = DeviceUUID;
	Info.State = EDeviceConnectType::UnConnected;
	Info.LEDColor1 = FColor::Black;
	Info.LEDColor2 = FColor::Black;
	return Info;
}

void UCustomDevice::AddToDeviceList(const FBLEDeviceInfo& Device)
{
	DeviceList.Add(Device);
	NotifyDeviceListChangedEvent();
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
#if PLATFORM_ANDROID
	bool bAllGranted = true;
	for (bool Result : GrantResults)
	{
		if (!Result)
		{
			bAllGranted = false;
			break;
		}
	}

	if (bAllGranted)
	{
		FindDeviceByServices();
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
	// UUIDで先にデバイスを区別したいが、なぜか何一つデバイスを検知できない
	//Services.Add(IO_SERVICE_UUID);
}

void UCustomDevice::FindDeviceByServices()
{
#if PLATFORM_ANDROID
	if (BleManager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Start Find Device"));
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
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Not Target Device"));
			return;
		}

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
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, FString::Printf(TEXT("Connect to Device: %s"), *Device.GetInterface()->GetDeviceName()));

			// つなげそうなデバイス見つけたら、DevicesWaitingに入れてLEDの色を確認する
			FString DeviceName = Device.GetInterface()->GetDeviceName();
			FString DeviceUUID = Device.GetInterface()->GetDeviceId();
			FBLEDeviceInfo PendingDevice = MakeDeviceBaseInfo(Device, DeviceName, DeviceUUID);
			DevicesWaiting.Add(PendingDevice);
			
			CurrentDevice = Device;
			CurrentDeviceInfo = &DevicesWaiting.Last();

			Connect();

			//　デバイスが１つのみの場合
			StopScanDevices();
		}
	}
#endif
}

void UCustomDevice::OnConnectSucc()
{
#if PLATFORM_ANDROID
	UE_LOG(LogTemplateDevice, Display, TEXT("Connect to device successfully"));
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Connect to device successfully"));

	Name = CurrentDevice.GetInterface()->GetDeviceName();
	UUID = CurrentDevice.GetInterface()->GetDeviceId();
	State = EDeviceConnectType::Connected;
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, FString::Printf(TEXT("Connect Success to Device: %s"), *Name));

	// コールバックを一度だけ登録する（接続成功時に必ず設定）
	FBleCharacteristicDataDelegate ReceiveFunction;
	ReceiveFunction.BindUFunction(this, FName("OnReceiveData"));
	CurrentDevice.GetInterface()->BindToCharacteristicNotificationEvent(ReceiveFunction);

	FBleCharacteristicDataDelegate ReadFunction;
	ReceiveFunction.BindUFunction(this, FName("OnReceiveData"));
	CurrentDevice.GetInterface()->BindToCharacteristicReadEvent(ReceiveFunction);

	FBleCharacteristicDelegate WriteFunction;
	WriteFunction.BindUFunction(this, FName("OnWriteData"));
	CurrentDevice.GetInterface()->BindToCharacteristicWriteEvent(WriteFunction);

	// まずどの段階にいるかを確認
	if (bIsMakeList)
	{
		GetValidationCode();
	}
	else
	{
		// デバイスの色を確認
		GetValidationCode();
		// つなぐデバイスが決めた
		//SendPairRequest(1);
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
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Disconnect to device successfully"));
	Name.Empty();
	UUID.Empty();
	State = EDeviceConnectType::UnConnected;
	CurrentDeviceInfo = nullptr;
	CurrentDevice = nullptr;
	BleManager = nullptr;
	LEDCheckTimer = LEDCheckTimeDuration;
	if (bNeedReScan)
	{
		ResetDeviceList();
		// 3秒後にスキャンを開始する、デバイスが切断されてすぐスキャンを開始すると、デバイスがまだ切断されていない状態でスキャンしてしまうため、見つけられない可能性がある
		FTimerHandle RescanTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(RescanTimerHandle, [this]() {
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Rescan devices"));
			Init();
		}, 5.0f, false);
	}
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
	// Queueに読み取り操作を積む
	EnqueueOperation(FBleOperation::MakeRead(IO_PAIR_SERVICE_UUID, IO_LED_COLOR_CHARACTERISTIC_UUID));
	//UE_LOG(LogTemplateDevice, Error, TEXT("Enqueue Read : %s, %s"), IO_PAIR_SERVICE_UUID, IO_LED_COLOR_CHARACTERISTIC_UUID);
#endif
}

void UCustomDevice::SendPairRequest(uint8 Enable)
{
#if PLATFORM_ANDROID
	UE_LOG(LogTemplateDevice, Display, TEXT("Do Write pair"));
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Do Write pair"));
	
	// ペアリング書き込みをQueueに積む
	TArray<uint8> Datas;
	Datas.Add(Enable);
	EnqueueOperation(FBleOperation::MakeWrite(IO_PAIR_SERVICE_UUID, IO_PAIR_CHARACTERISTIC_UUID, Datas));
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
	UE_LOG(LogTemplateDevice, Log, TEXT("OnWriteData completed: %s, %s"), *ServiceUUID, *CharacteristicUUID);
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, FString::Printf(TEXT("OnWrite Data completed: %s, %s"), *ServiceUUID, *CharacteristicUUID));

	// Pair書き込み完了 → Subscribe操作をQueueに追加してから次へ進む
	if (ServiceUUID.Equals(IO_PAIR_SERVICE_UUID) && CharacteristicUUID.Equals(IO_PAIR_CHARACTERISTIC_UUID))
	{
		// 現在の操作完了を通知する前に、次に必要な操作を積んでおく
		EnqueueOperation(FBleOperation::MakeSubscribe(IO_BIKE_SERVICE_UUID, IO_RPM_CHARACTERISTIC_UUID, false));
		EnqueueOperation(FBleOperation::MakeSubscribe(IO_BIKE_SERVICE_UUID, IO_REVOLUTION_CHARACTERISTIC_UUID, false));
	}

	// 現在の操作が終わったので次を処理する
	OnOperationCompleted();
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
	{
		HandleLEDData(Data);
		//Disconnect();
		OnOperationCompleted();
	}
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
	// 無線だと直接数字がもらえる、変換しなくていい
	//uint32 Revolution = TransformDataToInt<uint32>(Data.GetData(), Data.Num());
	uint16 Revolution = Data[0];
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(TEXT("Revolution: %d"), Revolution));
}

void UCustomDevice::HandleLEDData(const TArray<uint8>& Data)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("Get Data Length: %d"), Data.Num()));
	//for(int index = 0; index < Data.Num(); index++)
	//	GEngine->AddOnScreenDebugMessage(-1, 150.f, FColor::Orange, FString::Printf(TEXT("Data: %d, %d"), (int)Data[index], index));
	// デバイスからの色の順番はARGBの順番で、Data[3]がA、Data[2]がR、Data[1]がG、Data[0]がB
	// 赤色 0x00FF0000
	// 緑色 0x0000FF00
	// 青色 0x000000FF
	// 黄色 0x00FFFF00
	// シアン色 0x0000FFFF
	// マゼンタ色 0x00FF00FF
	// 白色 0x00FFFFFF
	FBLEDeviceInfo Target = DevicesWaiting.HeapTop();
	FColor LED[2];
	for (int i = 0; i < 2; i++)
	{
		int BaseIndex = i * 4;
		// αをそのまま使うと透明で見えないため、255に固定する
		LED[i] = FColor(Data[BaseIndex + 2], Data[BaseIndex + 1], Data[BaseIndex + 0], 255);
	}
	Target.LEDColor1 = LED[0];
	Target.LEDColor2 = LED[1];
	GEngine->AddOnScreenDebugMessage(-1, 150.f, FColor::Orange, FString::Printf(TEXT("LED 1: %s, LED 2: %s"), *Target.LEDColor1.ToString(), *Target.LEDColor2.ToString()));
	AddToDeviceList(Target);
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

void UCustomDevice::NotifyDeviceInfoChangedEvent()
{
	if (OnDeviceInfoChanged.IsBound())
		OnDeviceInfoChanged.Broadcast(*CurrentDeviceInfo);
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

//===================================================
// BLE Operation Queue 実装
//===================================================

void UCustomDevice::EnqueueOperation(const FBleOperation& Operation)
{
	OperationQueue.Enqueue(Operation);
	UE_LOG(LogTemplateDevice, Log, TEXT("EnqueueOperation: Type=%d, Service=%s, Characteristic=%s"),
		(int32)Operation.Type, *Operation.ServiceUUID, *Operation.CharacteristicUUID);
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, FString::Printf(TEXT("EnqueueOperation: Type = %d, Service = %s, Characteristic = %s"), (int32)Operation.Type, *Operation.ServiceUUID, *Operation.CharacteristicUUID));

	// 実行中でなければすぐに処理を開始する
	if (!bIsOperationInProgress)
	{
		ProcessNextOperation();
	}
}

void UCustomDevice::ProcessNextOperation()
{
#if PLATFORM_ANDROID
	// Queueが空なら何もしない
	if (OperationQueue.IsEmpty())
	{
		UE_LOG(LogTemplateDevice, Log, TEXT("OperationQueue is empty, nothing to process."));
		return;
	}

	// デバイスが接続されていないなら実行しない
	if (!CurrentDevice || State != EDeviceConnectType::Connected)
	{
		UE_LOG(LogTemplateDevice, Warning, TEXT("ProcessNextOperation: Device is not connected. Skipping."));
		return;
	}

	FBleOperation NextOp;
	if (!OperationQueue.Dequeue(NextOp))
		return;

	bIsOperationInProgress = true;

	UE_LOG(LogTemplateDevice, Log, TEXT("ProcessNextOperation: Type=%d, Service=%s, Characteristic=%s"),
		(int32)NextOp.Type, *NextOp.ServiceUUID, *NextOp.CharacteristicUUID);
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, FString::Printf(TEXT("ProcessNextOperation: Type = %d, Service = %s, Characteristic = %s"), (int32)NextOp.Type, *NextOp.ServiceUUID, *NextOp.CharacteristicUUID));

	switch (NextOp.Type)
	{
	case EBleOperationType::WriteCharacteristic:
	{
		CurrentDevice.GetInterface()->WriteCharacteristic(
			NextOp.ServiceUUID,
			NextOp.CharacteristicUUID,
			NextOp.Data
		);
		// 完了は OnWriteData コールバックで検知 → OnOperationCompleted() を呼ぶ
		break;
	}

	case EBleOperationType::SubscribeCharacteristic:
	{
		CurrentDevice.GetInterface()->SubscribeToCharacteristic(
			NextOp.ServiceUUID,
			NextOp.CharacteristicUUID,
			NextOp.bWithResponse
		);
		// SubscribeToCharacteristic は完了コールバックがないため、
		// 発行後すぐに完了扱いにして次へ進む
		OnOperationCompleted();
		break;
	}

	case EBleOperationType::ReadCharacteristic:
	{
		CurrentDevice.GetInterface()->ReadCharacteristic(
			NextOp.ServiceUUID,
			NextOp.CharacteristicUUID
		);
		// 完了は OnReceiveData コールバックで検知 → OnOperationCompleted() を呼ぶ
		break;
	}

	default:
		UE_LOG(LogTemplateDevice, Error, TEXT("ProcessNextOperation: Unknown operation type."));
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("ProcessNextOperation: Unknown operation type. %d"), (int32)NextOp.Type));
		OnOperationCompleted();
		break;
	}
#endif
}

void UCustomDevice::OnOperationCompleted()
{
	bIsOperationInProgress = false;
	UE_LOG(LogTemplateDevice, Log, TEXT("OnOperationCompleted: Moving to next operation."));

	// 次の操作があれば続けて処理する
	ProcessNextOperation();
}

void UCustomDevice::ClearOperationQueue()
{
	// TQueue はクリア用メソッドがないので空になるまで Dequeue する
	FBleOperation Dummy;
	while (OperationQueue.Dequeue(Dummy)) {}

	bIsOperationInProgress = false;
	UE_LOG(LogTemplateDevice, Log, TEXT("ClearOperationQueue: Queue cleared."));
}