// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include "CustomDevice.generated.h"

//===================================================
// BLE IO_SERVICE
//===================================================
// Service UUID
#define IO_BIKE_SERVICE_UUID "682a0468-1482-63be-dc47-4296d65ad4b8"
#define IO_PAIR_SERVICE_UUID "52f93fe3-013b-4256-8d87-fdcea414b533"
#define IO_DEVICE_NAME "BicycleDevice"
// Write Characteristic UUID
#define IO_REVOLUTION_RESET_CHARACTERISTIC_UUID "682a0468-1482-63be-dc47-4296d65ad4bc"
#define IO_PAIR_CHARACTERISTIC_UUID "abaae5f2-4829-121c-1595-0e80c6f0e78a"
// Read Characteristic UUID
#define IO_RPM_CHARACTERISTIC_UUID "682a0468-1482-63be-dc47-4296d65ad4b9"
#define IO_RPS_CHARACTERISTIC_UUID "682a0468-1482-63be-dc47-4296d65ad4ba"
#define IO_REVOLUTION_CHARACTERISTIC_UUID "682a0468-1482-63be-dc47-4296d65ad4bd"
#define IO_LED_COLOR_CHARACTERISTIC_UUID "df59a71f-82c2-0f07-1fdf-f7777c822823"

// Android Permission
#define ANDROID_FILE_LOCATION_PERMISSION "android.permission.ACCESS_FINE_LOCATION"
#define ANDROID_BLUETOOTH_CONNECT_PERMISSION "android.permission.BLUETOOTH_CONNECT"
#define ANDROID_BLUETOOTH_SCAN_PERMISSION "android.permission.BLUETOOTH_SCAN"

class IBleManagerInterface;
class IBleDeviceInterface;
struct FBLEDeviceInfo;

/// <summary>
/// BLE操作の種類
/// </summary>
UENUM()
enum class EBleOperationType : uint8
{
	WriteCharacteristic,       // データ書き込み（WriteCharacteristic）
	SubscribeCharacteristic,   // 通知購読（SubscribeToCharacteristic）
	ReadCharacteristic,        // データ読み取り（ReadCharacteristic）
};

/// <summary>
/// Queueに積む一つのBLE操作を表す構造体
/// </summary>
struct FBleOperation
{
	EBleOperationType Type;
	FString ServiceUUID;
	FString CharacteristicUUID;
	TArray<uint8> Data;         // WriteCharacteristic のみ使用
	bool bWithResponse = false; // SubscribeCharacteristic のみ使用

	// WriteCharacteristic 用
	static FBleOperation MakeWrite(const FString& Service, const FString& Characteristic, const TArray<uint8>& WriteData)
	{
		FBleOperation Op;
		Op.Type = EBleOperationType::WriteCharacteristic;
		Op.ServiceUUID = Service;
		Op.CharacteristicUUID = Characteristic;
		Op.Data = WriteData;
		return Op;
	}

	// SubscribeCharacteristic 用
	static FBleOperation MakeSubscribe(const FString& Service, const FString& Characteristic, bool bResponse = false)
	{
		FBleOperation Op;
		Op.Type = EBleOperationType::SubscribeCharacteristic;
		Op.ServiceUUID = Service;
		Op.CharacteristicUUID = Characteristic;
		Op.bWithResponse = bResponse;
		return Op;
	}

	// ReadCharacteristic 用
	static FBleOperation MakeRead(const FString& Service, const FString& Characteristic)
	{
		FBleOperation Op;
		Op.Type = EBleOperationType::ReadCharacteristic;
		Op.ServiceUUID = Service;
		Op.CharacteristicUUID = Characteristic;
		return Op;
	}
};

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UCustomDevice : public UDevice, public FTickableGameObject
{
	GENERATED_BODY()

public:

	UCustomDevice();
	~UCustomDevice();

	virtual void Init() override;

	void Enable() override;

	virtual bool Connect() override;

	virtual bool Disconnect() override;

	void EnableMoveAction_Implementation() override;

	void DisableMoveAction_Implementation() override;

	void EnableSelectAnswerAction_Implementation() override;

	void DisableSelectAnswerAction_Implementation() override;

	void ResetRevolution();

#pragma region TickableGameObject
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	virtual bool IsTickableInEditor() const override;
#pragma endregion

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeviceListChangedDelegate, const TArray<FBLEDeviceInfo>&, Devices);

	/// <summary>
	/// デバイスリストが変更された通知
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FDeviceListChangedDelegate OnDeviceListChanged;

private:

	/// <summary>
	/// Bluetooth周りの検査
	/// </summary>
	/// <returns></returns>
	bool CheckBluetooth();

	/// <summary>
	/// デバイスのLEDの色を確認
	/// </summary>
	void CheckDeviceLED();

	/// <summary>
	/// デバイスリストをリセット
	/// </summary>
	void ResetDeviceList();

	/// <summary>
	/// デバイスの基本情報を作る
	/// </summary>
	/// <param name="Device">インターフェイス</param>
	/// <param name="DeviceName">名前</param>
	/// <param name="DeviceUUID">UUID</param>
	FBLEDeviceInfo MakeDeviceBaseInfo(TScriptInterface<IBleDeviceInterface> Device, FString DeviceName, FString DeviceUUID);

	/// <summary>
	/// デバイスリストに追加
	/// </summary>
	/// <param name="Device">デバイス</param>
	void AddToDeviceList(FBLEDeviceInfo Device);

	/// <summary>
	/// アンドロイドのbluetoothの権限を要求する
	/// </summary>
	void RequestAndroidPermission();

	/// <summary>
	/// 権限の要求結果
	/// </summary>
	/// <param name="Permissions"></param>
	/// <param name="GrantResults"></param>
	UFUNCTION()
	void OnPermissionResult(const TArray<FString>& Permissions, const TArray<bool>& GrantResults);

	/// <summary>
	/// 目標のサービスを決める
	/// </summary>
	void DecideTargetServices();

	/// <summary>
	/// 周囲のデバイスを探す
	/// </summary>
	void FindDeviceByServices();

	/// <summary>
	/// 新しいデバイスが見つかった時
	/// </summary>
	/// <param name="Device">デバイス</param>
	UFUNCTION()
	void OnDeviceFound(TScriptInterface<IBleDeviceInterface> Device);

	/// <summary>
	/// コネクションが成功した時
	/// </summary>
	UFUNCTION()
	void OnConnectSucc();

	/// <summary>
	/// コネクションが失敗した時
	/// </summary>
	/// <param name="ErrorMessage">エラーメッセージ</param>
	UFUNCTION()
	void OnConnectError(FString ErrorMessage);

	/// <summary>
	/// 切断成功した時
	/// </summary>
	UFUNCTION()
	void OnDisconnectSucc();

	/// <summary>
	/// 切断失敗した時
	/// </summary>
	/// <param name="ErrorMessage">エラーメッセージ</param>
	UFUNCTION()
	void OnDisconnectError(FString ErrorMessage);

	/// <summary>
	/// 認証コードをゲット
	/// </summary>
	void GetValidationCode();

	/// <summary>
	/// ペアリクエストを送る
	/// </summary>
	void SendPairRequest();

	/// <summary>
	/// 貰ったデータから必要な数値に変換
	/// </summary>
	/// <param name="Data">データ</param>
	/// <param name="Size">データサイズ</param>
	/// <returns>数値</returns>
	template<typename T>
	T TransformDataToInt(const uint8_t* Data, int Size) const;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="ServiceUUID"></param>
	/// <param name="CharacteristicUUID"></param>
	UFUNCTION()
	void OnWriteData(FString ServiceUUID, FString CharacteristicUUID);

	/// <summary>
	/// 通知が来た時
	/// </summary>
	/// <param name="ServiceUUID">サービスUUID</param>
	/// <param name="CharacteristicUUID"></param>
	/// <param name="Data">データ</param>
	UFUNCTION()
	void OnReceiveData(FString ServiceUUID, FString CharacteristicUUID, TArray<uint8>& Data);

	/// <summary>
	/// RPMデータを処理
	/// </summary>
	/// <param name="Data">データ</param>
	void HandleRPMData(const TArray<uint8>& Data);

	/// <summary>
	/// RPSデータを処理
	/// </summary>
	/// <param name="Data">データ/param>
	void HandleRPSData(const TArray<uint8>& Data);

	/// <summary>
	/// 回転数データを処理
	/// </summary>
	/// <param name="Data">データ</param>
	void HandleRevolutionData(const TArray<uint8>& Data);

	/// <summary>
	/// ペアリングのLED色データを処理
	/// </summary>
	/// <param name="Data">色データ</param>
	void HandleLEDData(const TArray<uint8>& Data);

	/// <summary>
	/// もらったデータを表示する(デバッグ用)
	/// </summary>
	/// <param name="Data">データ</param>
	void DebugReceiveData(const TArray<uint8>& Data);

	/// <summary>
	/// 
	/// </summary>
	void NotifyDeviceListChangedEvent();

	/// <summary>
	/// 移動イベントを通知
	/// </summary>
	/// <param name="MoveData">移動量</param>
	void NotifyMoveEvent(FVector2D MoveData);

	/// <summary>
	/// 回転数イベントを通知
	/// </summary>
	/// <param name="MoveNum"></param>
	void NotifyMoveNumEvent(int MoveNum);

	/// <summary>
	/// 最大回転数を更新
	/// </summary>
	/// <param name="Standard">標準値</param>
	/// <param name="Danger">危険値</param>
	/// <param name="Safe">安全値</param>
	UFUNCTION()
	void UpdateMaxRPM(int Standard, int Danger, int Safe);

	//===================================================
	// BLE Operation Queue 関連
	//===================================================

	/// <summary>
	/// 操作をQueueに追加する
	/// </summary>
	/// <param name="Operation">追加する操作</param>
	void EnqueueOperation(const FBleOperation& Operation);

	/// <summary>
	/// Queueの次の操作を実行する
	/// 実行中の操作がある場合は何もしない
	/// </summary>
	void ProcessNextOperation();

	/// <summary>
	/// 現在実行中の操作を完了として、次の操作を処理する
	/// Write/Subscribe/Readの各コールバックから呼ぶ
	/// </summary>
	void OnOperationCompleted();

	/// <summary>
	/// Queueをクリアして実行中フラグをリセットする（切断時などに使用）
	/// </summary>
	void ClearOperationQueue();

	/// <summary>
	/// BLEマネジャー
	/// </summary>
	UPROPERTY()
	TScriptInterface<IBleManagerInterface> BleManager;

	/// <summary>
	/// 目標のサービス
	/// </summary>
	TArray<FString> Services;

	/// <summary>
	/// 目標のデバイス(今のところ一つしかない)
	/// </summary>
	UPROPERTY()
	TScriptInterface<IBleDeviceInterface> CurrentDevice;

	/// <summary>
	/// LEDデータがまだもらってないデバイス
	/// </summary>
	TArray<FBLEDeviceInfo> DevicesWaiting;

	/// <summary>
	/// n秒ごとデバイスのLEDを確認
	/// </summary>
	float LEDCheckTimeDuration = 0.5f;

	/// <summary>
	/// デバイスのLEDを確認するタイマー
	/// </summary>
	float LEDCheckTimer = 0.0f;

	/// <summary>
	/// つなげるデバイスリスト
	/// </summary>
	TArray<FBLEDeviceInfo> DeviceList;

	/// <summary>
	/// リストを用意する段階のフラグ
	/// </summary>
	bool bIsMakeList;

	/// <summary>
	/// 移動機能のスイッチ
	/// </summary>
	bool bMoveSwitch;

	/// <summary>
	/// 最大回転数
	/// </summary>
	float MaxRPM;

	/// <summary>
	/// BLE操作のQueue本体
	/// </summary>
	TQueue<FBleOperation> OperationQueue;

	/// <summary>
	/// 現在操作を実行中かどうか（trueの間は次の操作を開始しない）
	/// </summary>
	bool bIsOperationInProgress;
};
