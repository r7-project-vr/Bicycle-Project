// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include "CustomDevice.generated.h"

//===================================================
// BLE IO_SERVICE
//===================================================
// Service UUID
#define IO_SERVICE_UUID "3c3e3e6a-8916-4f28-95c1-09e5ef9c8e4b"
// Write Characteristic UUID
#define IO_WRITE_CHARACTERISTIC_UUID "72063c8c-a816-4e9a-8627-e9a689952370"
// Read Characteristic UUID
#define IO_READ_CHARACTERISTIC_UUID "770cd59f-4735-455d-a42a-acd1ac31e3dd"

//
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
	/// MappingContextでmoveアクションに設定されたキーを押したら最初に値がもらうところ
	/// </summary>
	/// <param name="Value"></param>
	void OnMove();

	/// <summary>
	/// 移動イベントを通知
	/// </summary>
	/// <param name="MoveData">移動量</param>
	void NotifyMoveEvent(FVector2D MoveData);

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

	FTimerHandle GetDataHandler;
};
