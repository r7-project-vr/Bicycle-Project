// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/DeviceCmdSender.h"

DeviceCmdSender::DeviceCmdSender(UASerialLibControllerWin* InDevice, TQueue<uint8_t, EQueueMode::Spsc>* InCommandQueue, TQueue<ASerialDataStruct::ASerialData, EQueueMode::Spsc>* InDataQueue)
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
		// チェックコマンドを送信する前に、チェックコマンドが既に送信されているか確認
		if(HasSendCheckCmd)
		{
			UE_LOG(LogTemp, Warning, TEXT("Check command is already sent, skipping this command"));
			return false;
		}
		// チェックコマンドを送信
		if (!SendCommand(Device, Command))
			return false;
		HasSendCheckCmd = true;

		// デバイスからの応答を待つ
		ASerialDataStruct::ASerialData FlagData;
		if (!WaitForDeviceResponse(Device, FlagData, &HasSendCheckCmd))
			return false;

		DataQueue->Enqueue(FlagData);
	}
	else if(Command == 0x21)
	{
		// RPMのコマンドを送信する前に、RPMコマンドが既に送信されているか確認
		if (HasSendRPMCmd)
		{
			UE_LOG(LogTemp, Warning, TEXT("RPM command is already sent, skipping this command"));
			return false;
		}
		// RPMコマンドを送信
		if (!SendCommand(Device, Command))
			return false;
		HasSendRPMCmd = true;
		
		ASerialDataStruct::ASerialData RPMData;
		if (!WaitForDeviceResponse(Device, RPMData, &HasSendRPMCmd))
			return false;
	
		DataQueue->Enqueue(RPMData);
	}
	return true;
}

bool DeviceCmdSender::SendCommand(UASerialLibControllerWin* Device, uint8_t Command)
{
	int WriteResult = Device->WriteData(Command);
	if (WriteResult != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to write %d command to device, result: %d"), Command, WriteResult);
		return false;
	}
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("Send command Success"));
	return true;
}

bool DeviceCmdSender::WaitForDeviceResponse(UASerialLibControllerWin* Device, ASerialDataStruct::ASerialData& oData, bool* flag)
{
	int ReadResult;
	do {
		ReadResult = Device->ReadDataProcess(&oData);
	} while (ReadResult == 0);

	if (flag)
	{
		*flag = false;
	}
	if (ReadResult < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read data from device"));
		return false;
	}
	return true;
}
