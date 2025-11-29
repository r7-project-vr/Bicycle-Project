// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include "CustomDevice.generated.h"

//===================================================
// BLE IO_SERVICE
//===================================================
// Service UUID
#define IO_SERVICE_UUID "682a0468-1482-63be-dc47-4296d65ad4b8"
// Write Characteristic UUID
#define IO_WRITE_CHARACTERISTIC_UUID "72063c8c-a816-4e9a-8627-e9a689952370"
// Read Characteristic UUID
#define IO_RPM_CHARACTERISTIC_UUID "682a0468-1482-63be-dc47-4296d65ad4b9"
#define IO_RPS_CHARACTERISTIC_UUID "682a0468-1482-63be-dc47-4296d65ad4ba"
#define IO_REVOLUTION_CHARACTERISTIC_UUID "682a0468-1482-63be-dc47-4296d65ad4bd"

// Android Permission
#define ANDROID_FILE_LOCATION_PERMISSION "android.permission.ACCESS_FILE_LOCATION"
#define ANDROID_BLUETOOTH_CONNECT_PERMISSION "android.permission.BLUETOOTH_CONNECT"
#define ANDROID_BLUETOOTH_SCAN_PERMISSION "android.permission.BLUETOOTH_SCAN"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UCustomDevice : public UDevice
{
	GENERATED_BODY()

public:

	UCustomDevice();

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
	void OnDeviceFound(TScriptInterface<class IBleDeviceInterface> Device);

	/// <summary>
	/// コネクションが成功した時
	/// </summary>
	void OnConnectSucc();

	/// <summary>
	/// コネクションが失敗した時
	/// </summary>
	/// <param name="ErrorMessage">エラーメッセージ</param>
	void OnConnectError(FString ErrorMessage);

	/// <summary>
	/// 切断成功した時
	/// </summary>
	void OnDisconnectSucc();

	/// <summary>
	/// 切断失敗した時
	/// </summary>
	/// <param name="ErrorMessage">エラーメッセージ</param>
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
	void OnNotification(FString ServiceUUID, FString CharacteristicUUID, TArray<uint8>& Data);

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
	class IBleManagerInterface* BleManager;

	/// <summary>
	/// 目標のサービス
	/// </summary>
	TArray<FString> Services;

	/// <summary>
	/// 目標のデバイス(今のところ一つしかない)
	/// </summary>
	IBleDeviceInterface* MyDevice;

	/// <summary>
	/// 移動機能のスイッチ
	/// </summary>
	bool MoveSwitch;

	/// <summary>
	/// 最大回転数
	/// </summary>
	float MaxRPM;
};
