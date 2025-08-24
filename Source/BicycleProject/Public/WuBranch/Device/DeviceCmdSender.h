// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#if PLATFORM_WINDOWS

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "ASerialLibControllerWin.h"
#include "Containers/Queue.h"


/**
 * 
 */
class BICYCLEPROJECT_API DeviceCmdSender : public FRunnable
{
public:
	DeviceCmdSender(UASerialLibControllerWin* InDevice, TQueue<uint8_t, EQueueMode::Spsc>* InCommandQueue, TQueue<ASerialDataStruct::ASerialData, EQueueMode::Spsc>* InDataQueue);
	~DeviceCmdSender();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	/// <summary>
	/// 安全に終了する
	/// </summary>
	void EnsureCompletion();

	/// <summary>
	/// 一時停止する
	/// </summary>
	void Pause();

	/// <summary>
	/// 再開する
	/// </summary>
	void ReStart();

private:

	/// <summary>
	/// コマンドを実行する
	/// </summary>
	/// <param name="Command"></param>
	bool DoCommand(uint8_t Command);

	/// <summary>
	/// RPMデータを取得する
	/// </summary>
	void GetRPMData();

	/// <summary>
	/// コマンドをデバイスに送信する
	/// </summary>
	/// <param name="Device">デバイス</param>
	/// <param name="Command">コマンド</param>
	/// <returns>true: 成功, false: 失敗</returns>
	bool SendCommand(UASerialLibControllerWin* Device, uint8_t Command);

	/// <summary>
	/// デバイスからの応答を待つ
	/// </summary>
	/// <param name="Device">デバイス</param>
	/// <param name="Data">データ</param>
	/// <param name="flag">フラッグ</param>
	/// <returns>true: 成功, false: 失敗</returns>
	bool WaitForDeviceResponse(UASerialLibControllerWin* Device, ASerialDataStruct::ASerialData& oData, bool* flag = nullptr);

	/// <summary>
	/// 今のスレッドを管理する
	/// </summary>
	FRunnableThread* Thread;

	/// <summary>
	/// 目標のデバイス
	/// </summary>
	TWeakObjectPtr<UASerialLibControllerWin> DeviceWP;

	/// <summary>
	/// 指令を受けるキュー
	/// </summary>
	TQueue<uint8_t, EQueueMode::Spsc>* CommandQueue;

	/// <summary>
	/// データをメインスレッドに送るキュー
	/// </summary>
	TQueue<ASerialDataStruct::ASerialData, EQueueMode::Spsc>* DataQueue;

	/// <summary>
	/// 一時停止フラグ
	/// </summary>
	bool IsPause = false;

	/// <summary>
	/// 動けるか
	/// </summary>
	bool IsRunning = false;

	/// <summary>
	/// 頻度
	/// </summary>
	float Frequency = 60.0f;

	/// <summary>
	/// チェックのコマンド
	/// </summary>
	uint8_t CheckCmd = 0x20;

	/// <summary>
	/// RPMのコマンド
	/// </summary>
	uint8_t RPMCmd = 0x21;

	/// <summary>
	/// RPSコマンド
	/// </summary>
	uint8_t RPSCmd = 0x22;

	/// <summary>
	/// RPMコマンドを送ったか
	/// </summary>
	bool HasSendRPMCmd = false;

	/// <summary>
	/// 確認のコマンドを送ったか
	/// </summary>
	bool HasSendCheckCmd = false;

	/// <summary>
	/// チェックを実行するか
	/// </summary>
	bool NeedCheck = true;
};

#endif