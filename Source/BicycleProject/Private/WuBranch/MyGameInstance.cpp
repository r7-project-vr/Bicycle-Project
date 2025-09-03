// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Device/DeviceManager.h"
#include <UntakuBranch/Question.h>

UMyGameInstance::UMyGameInstance()
	: Coins(0)
	, IsClear(false)
	, MaxRPM(100)
	, StandardRPM(50)
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
	UpdateCoin();
}

void UMyGameInstance::SaveCoinsToFile()
{
}

void UMyGameInstance::SaveQuizsForResult(TArray<FQuestion> Result)
{
	Quizs = Result;
}

void UMyGameInstance::SetGameResult(bool Result)
{
	IsClear = Result;
}

int UMyGameInstance::GetMaxRPM() const
{
	return MaxRPM;
}

void UMyGameInstance::SetMaxRPM(int Value)
{
	MaxRPM = Value;
}

int UMyGameInstance::GetStandardRPM() const
{
	return StandardRPM;
}

void UMyGameInstance::SetStandardRPM(int Value)
{
	StandardRPM = Value;
}

void UMyGameInstance::ResetStandardRPM()
{
	StandardRPM = 50;
}

void UMyGameInstance::ReadCoinFromFile()
{
	Coins = 0; // 初期化
}

void UMyGameInstance::UpdateCoin()
{
	if (OnUpdateCoin.IsBound())
	{
		OnUpdateCoin.Broadcast(Coins);
	}
}
