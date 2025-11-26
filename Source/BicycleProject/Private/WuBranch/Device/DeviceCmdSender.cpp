// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/DeviceCmdSender.h"
#include "WuBranch/Device/ECommandType.h"

#if PLATFORM_WINDOWS

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
	CurrentState = EProcessState::Idle;
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

		double StartTime = FPlatformTime::Seconds();

		// RPM・RPS以外のコマンドの処理
		HandleCommand();

		GetRPMData();

		double Elapsed = FPlatformTime::Seconds() - StartTime;
		double SleepTime = 1.f / Frequency - Elapsed;

		if (SleepTime > 0)
		{
			// 更新頻度に応じてスリープ
			FPlatformProcess::Sleep(SleepTime);
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

void DeviceCmdSender::HandleCommand()
{
	UASerialLibControllerWin* Device = DeviceWP.Get();
	if (!Device)
		return;

	switch (CurrentState)
	{
	case EProcessState::Idle:
	{
		if (!CommandQueue->IsEmpty())
		{
			CurrentState = EProcessState::Sending;
		}
		break;
	}
	case EProcessState::Sending:
	{
		uint8_t Command;
		CommandQueue->Peek(Command);
		// コマンドを送信
		if (!SendCommand(Device, Command))
			return;

		// 返信する必要のないコマンド
		if (Command == (uint8_t)ECommandType::RevolutionsReset)
		{
			// コマンドを削除
			CommandQueue->Pop();
			// 次のコマンドを待つ状態に移行
			CurrentState = EProcessState::Idle;
			return;
		}

		// 返信を確認する。もし返信がない場合は待つ状態に移行
		ASerialDataStruct::ASerialData Data;
		if (WaitForDeviceResponse(Device, Data))
		{
			// 返信を貰った場合
			Data.command = Command;
			// データが正常に取得できた場合、キューに追加
			DataQueue->Enqueue(Data);
			// コマンドを削除
			CommandQueue->Pop();
			// 次のコマンドを待つ状態に移行
			CurrentState = EProcessState::Idle;
		}
		else
		{
			// 貰ってない場合、レスポンスを待つ状態に移行
			CurrentState = EProcessState::WaitingResponse;
		}
		break;
	}
	case EProcessState::WaitingResponse:
	{
		// チェックコマンドを送信
		if (!SendCommand(Device, (uint8_t)ECommandType::RPMUpdateCheck))
			return;

		ASerialDataStruct::ASerialData Data;
		if (WaitForDeviceResponse(Device, Data))
		{
			// データが正常に取得できた場合
			if (Data.data[0] == 1)
			{
				CurrentState = EProcessState::Sending;
			}
		}
		break;
	}
	}
}

void DeviceCmdSender::GetRPMData()
{
	UASerialLibControllerWin* Device = DeviceWP.Get();
	if (!Device)
		return;

	// RPMの更新フラグは内部で値の更新があると立ち上がります。
	// 逆に更新フラグの取得が行われたらフラグが立ち下がります。
	// RPM・RPSコマンドが送ったかどうかには関係ない
	// なので、先にチェックコマンドを送り、更新あるならRPM・RPSコマンドを送る

	ASerialDataStruct::ASerialData Data;
	if (NeedCheck)
	{
		// チェックコマンドを送信
		if (!SendCommand(Device, (uint8_t)ECommandType::RPMUpdateCheck))
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
		if (!SendCommand(Device, (uint8_t)ECommandType::RPM))
			return;

		if (!WaitForDeviceResponse(Device, Data))
			return;
			
		// データが正常に取得できた場合、キューに追加
		Data.command = (uint8_t)ECommandType::RPM;
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

#elif PLATFORM_ANDROID



#endif