// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include <ASerialCore/ASerialPacket.h>
#include "WiredDevice.generated.h"

class UASerialLibControllerWin;
class DeviceCmdSender;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UWiredDevice : public UDevice , public FTickableGameObject
{
	GENERATED_BODY()
	
public:

	UWiredDevice();
	virtual ~UWiredDevice() override;

	void Init(int DeviceID, int DeviceVer) override;

#pragma region TickableGameObject
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	virtual bool IsTickableInEditor() const override;
#pragma endregion

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

public:
	/// <summary>
	/// デバイスから移動のデータを取得する
	/// </summary>
	void GetMoveDataFromDevice();

private:
	
	/// <summary>
	/// RPMデータを受け取った時の処理
	/// </summary>
	/// <param name="RPMData">RPMデータ</param>
	void HandleRPMData(const ASerialDataStruct::ASerialData& RPMData);

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
	/// 実際使う装置
	/// </summary>
	UPROPERTY()
	UASerialLibControllerWin* Device;

	/// <summary>
	/// デバイスのコマンドを送るもの
	/// </summary>
	DeviceCmdSender* CmdSender;

	/// <summary>
	/// 実行待ちコマンドのキュー
	/// </summary>
	TQueue<uint8_t, EQueueMode::Spsc> CommandQueue;

	/// <summary>
	/// 貰ったデータのキュー
	/// </summary>
	TQueue<ASerialDataStruct::ASerialData, EQueueMode::Spsc> DataQueue;
	
};
