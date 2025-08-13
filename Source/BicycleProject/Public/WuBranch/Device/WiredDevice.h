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
class BICYCLEPROJECT_API UWiredDevice : public UDevice, public FTickableGameObject
{
	GENERATED_BODY()
	
public:

	UWiredDevice();

	void Init(int DeviceID, int DeviceVer) override;

	/// <summary>
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	virtual bool IsTickableInEditor() const override;

	virtual bool Connect() override;

	virtual bool Disconnect() override;

	void EnableMoveAction_Implementation() override;

	void DisableMoveAction_Implementation() override;

	void EnableSelectAnswerAction_Implementation() override;

	void DisableSelectAnswerAction_Implementation() override;

private:

	/// <summary>
	/// デバイスは既に初期化したか
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	bool CheckDevice();

	/// <summary>
	/// デバイスから移動のデータを取得する
	/// </summary>
	void GetMoveDataFromDevice();

	/// <summary>
	/// 貰ったデータから必要な数値に変換
	/// </summary>
	/// <param name="Data">データ</param>
	/// <param name="Size">データサイズ</param>
	/// <returns>数値</returns>
	int TransformDataToInt(const uint8_t* Data, int Size) const;

	/// <summary>
	/// 移動イベントを通知
	/// </summary>
	/// <param name="MoveData">移動量</param>
	void NotifyMoveEvent(FVector2D MoveData);

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
