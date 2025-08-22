// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Device/DeviceManager.h"

UMyGameInstance::UMyGameInstance()
	: Coins(0)
{
	DeviceManager = nullptr;
}

void UMyGameInstance::Init()
{
	Super::Init();

	if (!DeviceManager)
	{
		DeviceManager = NewObject<UDeviceManager>(this);
	}

	ReadCoinFromFile();
}

UDeviceManager* UMyGameInstance::GetDeviceManager() const
{
	return DeviceManager;
}

int UMyGameInstance::GetCoins() const
{
	return Coins;
}

void UMyGameInstance::AddCoins(int Amount)
{
	Coins += Amount;
	if (Coins < 0)
	{
		Coins = 0; // コインがマイナスにならないようにする
	}
}

void UMyGameInstance::SaveCoinsToFile()
{
}

void UMyGameInstance::ReadCoinFromFile()
{
	Coins = 0; // 初期化
}
