// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include "WiredDevice.generated.h"

class UASerialLibControllerWin;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UWiredDevice : public UDevice
{
	GENERATED_BODY()
	
public:

	UWiredDevice();

	void Init(int DeviceID, int DeviceVer);

	virtual bool Connect() override;

	virtual bool Disconnect() override;

	void EnableMoveAction_Implementation() override;

	void DisableMoveAction_Implementation() override;

	void EnableSelectAnswerAction_Implementation() override;

	void DisableSelectAnswerAction_Implementation() override;

	void GetData();

private:

	/// <summary>
	/// デバイスは既に初期化したか
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	bool CheckDevice();

	/// <summary>
	/// 移動機能のスイッチ
	/// </summary>
	bool MoveSwitch;

	/// <summary>
	/// 選ぶ機能のスイッチ
	/// </summary>
	bool SelectSwitch;

	/// <summary>
	/// 実際使う装置
	/// </summary>
	UPROPERTY()
	UASerialLibControllerWin* Device;
};
