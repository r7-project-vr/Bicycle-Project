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

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UCustomDevice : public UDevice
{
	GENERATED_BODY()

public:

	UCustomDevice();
	~UCustomDevice();

	virtual void Init() override;

	virtual bool Connect() override;

	virtual bool Disconnect() override;

	void EnableMoveAction_Implementation() override;

	void DisableMoveAction_Implementation() override;

	void EnableSelectAnswerAction_Implementation() override;

	void DisableSelectAnswerAction_Implementation() override;

private:

	/// <summary>
	/// Bluetooth周りの検査
	/// </summary>
	/// <returns></returns>
	bool CheckBluetooth();

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
	/// 貰ったデータから必要な数値に変換
	/// </summary>
	/// <param name="Data">データ</param>
	/// <param name="Size">データサイズ</param>
	/// <returns>数値</returns>
	template<typename T>
	T TransformDataToInt(const uint8_t* Data, int Size) const;

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
	TScriptInterface<IBleDeviceInterface> MyDevice;

	/// <summary>
	/// 移動機能のスイッチ
	/// </summary>
	bool MoveSwitch;

	/// <summary>
	/// 最大回転数
	/// </summary>
	float MaxRPM;
};
