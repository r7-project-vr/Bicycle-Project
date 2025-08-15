// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/DeviceCmdSender.h"

DeviceCmdSender::DeviceCmdSender(UASerialLibControllerWin* InDevice, TQueue<uint8_t, EQueueMode::Mpsc>* InCommandQueue, TQueue<ASerialDataStruct::ASerialData, EQueueMode::Mpsc>* InDataQueue)
	: Thread(nullptr)
	, DeviceWP(InDevice)
	, CommandQueue(InCommandQueue)
	, DataQueue(InDataQueue)
	, IsPause(false)
	, IsRunning(true)
{
	Thread = FRunnableThread::Create(this, TEXT("DeviceCmdSender"), 0, TPri_Normal);
}

DeviceCmdSender::~DeviceCmdSender()
{
	if(Thread)
	{
		EnsureCompletion();
		delete Thread;
		Thread = nullptr;
	}
}

bool DeviceCmdSender::Init()
{
	HasSendRPMCmd = false;
	return true;
}

uint32 DeviceCmdSender::Run()
{
	// キューの中にコマンドがあるか強制終了されてない限りループ
	while (IsRunning)
	{
		// 一時停止フラグが立っているか、コマンドキューが空ならばスキップ
		if (IsPause || CommandQueue->IsEmpty())
			continue;

		uint8_t Command;
		if (CommandQueue->Dequeue(Command))
		{
			if (!DoCommand(Command))
			{
				UE_LOG(LogTemp, Error, TEXT("Do command %d fail"), Command);
			}
		}
	}
	return 0;
}

void DeviceCmdSender::Stop()
{
	IsRunning = false;
}

void DeviceCmdSender::Exit()
{
}

void DeviceCmdSender::EnsureCompletion()
{
	Stop();

	// スレッドが存在する場合、完了を待つ
	if (Thread)
	{
		Thread->WaitForCompletion();
	}
}

void DeviceCmdSender::Pause()
{
	IsPause = true;
}

void DeviceCmdSender::ReStart()
{
	IsPause = false;
}

bool DeviceCmdSender::DoCommand(uint8_t Command)
{
	UASerialLibControllerWin* Device = DeviceWP.Get();
	if (!Device)
		return false;

	if (Command == 0x20)
	{
		int CheckResult = Device->WriteData(Command);
		if (CheckResult != 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to write check command to device, result: %d"), CheckResult);
			return false;
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("Send Check command"));
	}
	else if(Command == 0x21)
	{
		// RPMのコマンドを送信する前に、RPMコマンドが既に送信されているか確認
		if (HasSendRPMCmd)
			return false;
		// RPMコマンドを送信
		int WriteResult = Device->WriteData(Command);
		if (WriteResult != 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to write RPM command to device, result: %d"), WriteResult);
			return false;
		}
		HasSendRPMCmd = true;
		// RPMデータを貰う
		ASerialDataStruct::ASerialData RPMData;
		int ReadResult;
		do {
			ReadResult = Device->ReadDataProcess(&RPMData);
		} while (ReadResult == 0);
		HasSendRPMCmd = false;
		// データの読み取り結果をログに出力
		if (ReadResult < 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to read RPM data from device"));
			return false;
		}
		DataQueue->Enqueue(RPMData);
	}
	return true;
}
