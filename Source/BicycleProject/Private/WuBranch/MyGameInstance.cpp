// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Device/DeviceManager.h"
#include <UntakuBranch/Question.h>
#include "WuBranch/Actor/Animal.h"

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

	ReadAll();
}

#pragma region デバイス
UDeviceManager* UMyGameInstance::GetDeviceManager() const
{
	return DeviceManager;
}
#pragma endregion

#pragma region コイン
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

float UMyGameInstance::GetCoinHeight() const
{
	return CoinHeight;
}

void UMyGameInstance::SetCoinHeight(float Height)
{
	CoinHeight = Height;
	UpdateCoinHeight();
}

void UMyGameInstance::ResetCoinHeight()
{
	CoinHeight = 475.f;
	UpdateCoinHeight();
}

void UMyGameInstance::SaveCoinsToFile()
{

}

void UMyGameInstance::ReadCoinFromFile()
{
	Coins = 0; // 初期化
	CoinHeight = 475.f;
}

void UMyGameInstance::UpdateCoin()
{
	if (OnUpdateCoin.IsBound())
	{
		OnUpdateCoin.Broadcast(Coins);
	}
}

void UMyGameInstance::UpdateCoinHeight()
{
	if (OnUpdateCoinHeight.IsBound())
	{
		OnUpdateCoinHeight.Broadcast(CoinHeight);
	}
}
#pragma endregion

#pragma region ゲーム結果
void UMyGameInstance::SaveQuizsForResult(TArray<FQuestion*> Result)
{
	Quizs.Empty();
	for (FQuestion* Quiz : Result)
	{
		Quizs.Add(*Quiz);
	}
}

void UMyGameInstance::SetGameResult(bool Result)
{
	IsClear = Result;
}
#pragma endregion

#pragma region RPM
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
	NotifyUpdateStandardRPM();
}

void UMyGameInstance::ResetStandardRPM()
{
	StandardRPM = 50;
	NotifyUpdateStandardRPM();
}

void UMyGameInstance::NotifyUpdateStandardRPM()
{
	if (OnUpdateStandardRPM.IsBound())
		OnUpdateStandardRPM.Broadcast(StandardRPM);
}
#pragma endregion

#pragma region 動物
void UMyGameInstance::AddAnimal(TSubclassOf<AAnimal> Animal)
{
	// ヌール検査
	if (!Animal)
		return;

	// もう入れられない
	if (Animals.Num() >= MaxAnimalCount)
		return;

	// 追加
	Animals.Add(Animal);
}

void UMyGameInstance::RemoveAnimal(TSubclassOf<AAnimal> Animal)
{
	// ヌール検査
	if (!Animal)
		return;

	// 配列から見つけない
	if (Animals.Num() > 0 && Animals.Contains(Animal))
		return;

	// 削除
	//Animals.Remove(Animal);
}

TArray<TSubclassOf<AAnimal>> UMyGameInstance::GetAnimals() const
{
	return Animals;
}

void UMyGameInstance::SetMaxAnimalCount(int Amount)
{
	MaxAnimalCount = Amount;
}

void UMyGameInstance::SaveAnimalToFile()
{
}
#pragma endregion

#pragma region 自転車調整
void UMyGameInstance::SetBikeOffset(FVector Offset)
{
	BikeOffset = Offset;
}

FVector UMyGameInstance::GetBikeOffset() const
{
	return BikeOffset;
}
#pragma endregion

#pragma region セーブ
void UMyGameInstance::SaveAllToFile()
{
	SaveAnimalToFile();
	SaveCoinsToFile();
}

void UMyGameInstance::ReadAll()
{
	ReadCoinFromFile();
}
#pragma endregion
