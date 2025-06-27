// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include "CustomDevice.generated.h"

//===================================================
// BLE IO_SERVICE
//===================================================
//Service UUID
#define IO_SERVICE_UUID "3c3e3e6a-8916-4f28-95c1-09e5ef9c8e4b"

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

	void EnableDefaultActions_Implementation() override;

	void DisableDefaultActions_Implementation() override;

	void EnableSelectAnswerActions_Implementation() override;

	void DisableSelectAnswerActions_Implementation() override;

	void Connect_Implementation() override;

private:

	/// <summary>
	/// Bluetooth周りの検査
	/// </summary>
	/// <returns></returns>
	bool CheckBluetooth();

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

	void OnConnectSucc();

	/// <summary>
	/// MappingContextでmoveアクションに設定されたキーを押したら最初に値がもらうところ
	/// </summary>
	/// <param name="Value"></param>
	void OnMove();

	/// <summary>
	/// MappingContextで左の答えを選ぶアクションに設定されたキーを押したら最初に値がもらうところ
	/// </summary>
	/// <param name="Value"></param>
	void OnSelectLeftAnswer();

	/// <summary>
	/// MappingContextで右の答えを選ぶアクションに設定されたキーを押したら最初に値がもらうところ
	/// </summary>
	/// <param name="Value"></param>
	void OnSelectRightAnswer();

	/// <summary>
	/// BLEマネジャー
	/// </summary>
	IBleManagerInterface* BleManager;

	/// <summary>
	/// 目標のサービス
	/// </summary>
	TArray<FString> Services;

	/// <summary>
	/// 目標のデバイス(今のところ一つしかない)
	/// </summary>
	IBleDeviceInterface* MyDevice;

	/// <summary>
	/// デフォルトアクションのスイッチ
	/// </summary>
	bool DefaultActionSwitch;

	/// <summary>
	/// 答えを選択するアクションのスイッチ
	/// </summary>
	bool SelectAnswerSwitch;
};
