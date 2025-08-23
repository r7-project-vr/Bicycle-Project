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
		// 一時停止ならばスキップ
		if (IsPause)
			continue;

		/*uint8_t Command;
		if (CommandQueue->Dequeue(Command))
		{
			if (!DoCommand(Command))
			{
				UE_LOG(LogTemp, Error, TEXT("Do command %d fail"), Command);
			}
		}*/

		GetRPMData();

		// 頻度に応じてスリープ
		FPlatformProcess::Sleep(1.f / Frequency);
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

	if(Command == RPMCmd)
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
		UE_LOG(LogTemp, Log, TEXT("Send RPM command"));
		HasSendRPMCmd = true;

		// チェックする
		//CheckRPMState(Device);

		UE_LOG(LogTemp, Log, TEXT("RPM Data start"));
		ASerialDataStruct::ASerialData RPMData;
		if (!WaitForDeviceResponse(Device, RPMData, &HasSendRPMCmd))
			return false;
	
		UE_LOG(LogTemp, Log, TEXT("RPM Data complete"));
		DataQueue->Enqueue(RPMData);
	}
	return true;
}

void DeviceCmdSender::GetRPMData()
{
	UASerialLibControllerWin* Device = DeviceWP.Get();
	if (!Device)
		return;

	ASerialDataStruct::ASerialData Data;
	if (NeedCheck)
	{
		// チェックコマンドを送信
		if (!SendCommand(Device, CheckCmd))
			return;

		if (!WaitForDeviceResponse(Device, Data))
			return;
		
		// データが正常に取得できた場合
		if (Data.data[0] == 1)
		{
			NeedCheck = false;
		}
	}
	else
	{
		// RPMコマンドを送信
		if (!SendCommand(Device, RPMCmd))
			return;

		if (!WaitForDeviceResponse(Device, Data))
			return;

		// データが正常に取得できた場合、キューに追加
		DataQueue->Enqueue(Data);
		NeedCheck = true;
	}
}

bool DeviceCmdSender::SendCommand(UASerialLibControllerWin* Device, uint8_t Command)
{
	int WriteResult = Device->WriteData(Command);
	if (WriteResult != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to write %d command to device, result: %d"), Command, WriteResult);
		return false;
	}
	return true;
}

bool DeviceCmdSender::WaitForDeviceResponse(UASerialLibControllerWin* Device, ASerialDataStruct::ASerialData& oData, bool* flag)
{
	int ReadResult;
	//do {
		ReadResult = Device->ReadData(&oData);
	//} while (ReadResult == 0);
	
	if (flag)
	{
		*flag = false;
	}
	if (ReadResult == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read data from device"));
		return false;
	}
	else if (ReadResult == -2) 
	{
		UE_LOG(LogTemp, Error, TEXT("Read Timeout"));
		return false;
	}
	return true;
}
