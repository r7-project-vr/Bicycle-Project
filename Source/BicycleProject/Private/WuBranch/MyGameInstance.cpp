// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Device/DeviceManager.h"
#include <UntakuBranch/Question.h>
#include "WuBranch/Actor/Animal.h"

UMyGameInstance::UMyGameInstance()
	: TotalCoins(0)
	, IsClear(false)
	, StandardRPM(50)
	, RPMThreshold(10)
	, MaxStandardRPM(60)
	// 2025.11.09 谷村 start
	, NumOfSets(1)
	// 2025.11.09 谷村 end
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
int UMyGameInstance::GetTotalCoins() const
{
	return TotalCoins;
}

void UMyGameInstance::SetTotalCoins(int Amount)
{
	TotalCoins = FMath::Clamp(Amount, 0, INT32_MAX);
	UpdateCoin();
}

void UMyGameInstance::AddCoinsPerGame(int Amount)
{
	// 無効な値
	if(Amount <= 0)
		return;

	CoinsPerGame += Amount;
}

void UMyGameInstance::ResetCoinsPerGame()
{
	CoinsPerGame = 0;
}

int UMyGameInstance::GetCoinsPerGame() const
{
	return CoinsPerGame;
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
	TotalCoins = 0; // 初期化
	CoinHeight = 475.f;
}

void UMyGameInstance::UpdateCoin()
{
	if (OnUpdateCoin.IsBound())
	{
		OnUpdateCoin.Broadcast(TotalCoins);
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
void UMyGameInstance::SetStandardRPM(int Value)
{
	StandardRPM = FMath::Clamp(Value, 0, MaxStandardRPM);
	NotifyUpdateRPM();
}

int UMyGameInstance::GetStandardRPM() const
{
	return StandardRPM;
}

void UMyGameInstance::ResetStandardRPM()
{
	StandardRPM = 50;
	NotifyUpdateRPM();
}

void UMyGameInstance::AdjustThreshold(int Value)
{
	RPMThreshold += Value;
	NotifyUpdateRPM();
}

int UMyGameInstance::GetThreshold() const
{
	return RPMThreshold;
}

void UMyGameInstance::ResetThreshold()
{
	RPMThreshold = 10;
	NotifyUpdateRPM();
}

int UMyGameInstance::GetDangerRPM() const
{
	return StandardRPM + RPMThreshold;
}

int UMyGameInstance::GetSafeRPM() const
{
	return StandardRPM - RPMThreshold;
}

int UMyGameInstance::GetMaxRPM() const
{
	return MaxStandardRPM;
}

void UMyGameInstance::NotifyUpdateRPM()
{
	if (OnUpdateRPM.IsBound())
		OnUpdateRPM.Broadcast(GetStandardRPM(), GetDangerRPM(), GetSafeRPM());
}
#pragma endregion

// 2025.11.12 谷村 start
#pragma region セット数
void UMyGameInstance::SetNumOfSets(int Value)
{
	NumOfSets = Value;
}

int32 UMyGameInstance::GetNumOfSets() const
{
	return NumOfSets;
}
#pragma endregion
// 2025.11.12 谷村 end

#pragma region 動物
void UMyGameInstance::AddAnimal(TSubclassOf<AAnimal> Animal)
{
	// ヌール検査
	if (!Animal)
		return;

	// もう入れられない
	if (HasMaxAnimals())
		return;

	// 追加
	Animals.Add(Animal);
}

void UMyGameInstance::RemoveAnimal(TSubclassOf<AAnimal> Animal)
{
	// ヌール検査
	if (!Animal)
		return;

	// 配列のなかに見当たらない
	if (Animals.Num() > 0 && Animals.Contains(Animal))
		return;

	// 削除
	Animals.Remove(Animal);
}

TArray<TSubclassOf<AAnimal>> UMyGameInstance::GetAnimals() const
{
	return Animals;
}

void UMyGameInstance::SetMaxAnimalCount(int Amount)
{
	MaxAnimalCount = Amount;
}

bool UMyGameInstance::HasMaxAnimals() const
{
	return Animals.Num() >= MaxAnimalCount;
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
