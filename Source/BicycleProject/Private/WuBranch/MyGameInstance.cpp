// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Device/DeviceManager.h"
#include <UntakuBranch/Question.h>
#include "WuBranch/Actor/Animal.h"
#include <Kismet/GameplayStatics.h>
#include "WuBranch/Struct/PlayerSaveGame.h"
#include "WuBranch/System/SaveGameManager.h"

UMyGameInstance::UMyGameInstance()
	: TotalCoins(0)
	, CoinsPerGame(0)
	, CoinHeight(475.f)
	, IsClear(false)
	, StandardRPM(50)
	, RPMThreshold(10)
	, MaxStandardRPM(60)
	// 2025.11.09 谷村 start
	, NumOfSets(4)
	// 2025.11.09 谷村 end
	, MaxAnimalCount(10)
{
	DeviceManager = nullptr;
	FileName = TEXT("PlayerData.json");
}

void UMyGameInstance::Init()
{
	Super::Init();

	if (!DeviceManager)
	{
		DeviceManager = NewObject<UDeviceManager>(this);
	}

	GetSubsystem<USaveGameManager>()->OnLoadCompleted.AddDynamic(this, &UMyGameInstance::OnLoadComplete);
	GetSubsystem<USaveGameManager>()->OnSaveCompleted.AddDynamic(this, &UMyGameInstance::OnSaveComplete);

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
	CoinHeight = 100.f;
	UpdateCoinHeight();
}

void UMyGameInstance::SaveCoinsToFile(FPlayerSaveGame& Data)
{
	Data.Coins = TotalCoins;
}

void UMyGameInstance::ReadCoinFromFile(const FPlayerSaveGame& Data)
{
	TotalCoins = Data.Coins;
	CoinHeight = 100.f;
	UpdateCoinHeight();
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

void UMyGameInstance::SetGameResult(bool bResult)
{
	IsClear = bResult;
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

void UMyGameInstance::SaveRPMToFile(FPlayerSaveGame& Data)
{
	Data.RPMLimit = StandardRPM;
	Data.RPMThreshold = RPMThreshold;
}

void UMyGameInstance::ReadRPMFromFile(const FPlayerSaveGame& Data)
{
	StandardRPM = Data.RPMLimit;
	RPMThreshold = Data.RPMThreshold;
	MaxStandardRPM = 60;
	NotifyUpdateRPM();
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

// 2025.12.05 ウー start
void UMyGameInstance::SaveSetsToFile(FPlayerSaveGame& Data)
{
	Data.MapSets = NumOfSets;
}

void UMyGameInstance::ReadSetsFromFile(const FPlayerSaveGame& Data)
{
	NumOfSets = Data.MapSets;
}
// 2025.12.05 ウー end
#pragma endregion
// 2025.11.12 谷村 end

#pragma region 動物
void UMyGameInstance::AddAnimal(int32 AnimalID)
{
	// もう入れられない
	if (HasMaxAnimals())
		return;

	// 追加
	OwnedAnimals.Add(AnimalID);
}

void UMyGameInstance::RemoveAnimal(int32 AnimalID)
{
	if (OwnedAnimals.Num() <= 0)
		return;

	// 配列のなかに見当たらない
	if (!OwnedAnimals.Contains(AnimalID))
		return;

	// 削除
	OwnedAnimals.RemoveSingle(AnimalID);
}

TArray<TSubclassOf<AAnimal>> UMyGameInstance::GetAnimals() const
{
	return Animals;
}

TArray<int32> UMyGameInstance::GetOwnedAnimals() const
{
	return OwnedAnimals;
}

void UMyGameInstance::SetMaxAnimalCount(int Amount)
{
	MaxAnimalCount = Amount;
}

bool UMyGameInstance::HasMaxAnimals() const
{
	return OwnedAnimals.Num() >= MaxAnimalCount;
}

void UMyGameInstance::SaveAnimalToFile(FPlayerSaveGame& Data)
{
	Data.OwnedAnimals = OwnedAnimals;
}

void UMyGameInstance::ReadAnimalFromFile(const FPlayerSaveGame& Data)
{
	OwnedAnimals = Data.OwnedAnimals;
}
#pragma endregion

#pragma region 写真
void UMyGameInstance::AddAnimalPhoto(int32 AnimalID, int32 Nums)
{
	if (AnimalPhotoNums.Contains(AnimalID))
		AnimalPhotoNums[AnimalID] += Nums;
	else
		AnimalPhotoNums[AnimalID] = Nums;
}

void UMyGameInstance::ResetAnimalPhoto()
{
	AnimalPhotoNums.Empty();
}

void UMyGameInstance::SavePhotoToFile(FPlayerSaveGame& Data)
{
	Data.AnimalPhotos = AnimalPhotoNums;
}

void UMyGameInstance::ReadPhotoFromFile(const FPlayerSaveGame& Data)
{
	AnimalPhotoNums = Data.AnimalPhotos;
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
	// データ作り
	FPlayerSaveGame Data;
	SaveAnimalToFile(Data);
	SaveCoinsToFile(Data);
	SaveSetsToFile(Data);
	SaveRPMToFile(Data);
	SavePhotoToFile(Data);
	
	FString Path = FPaths::ProjectSavedDir() / FileName;
	GetSubsystem<USaveGameManager>()->SaveFile(Path, Data);
}

void UMyGameInstance::ReadAll()
{
	FString Path = FPaths::ProjectSavedDir() / FileName;
	GetSubsystem<USaveGameManager>()->LoadFile(Path);
}

void UMyGameInstance::OnSaveComplete(bool bResult)
{
	if (!bResult)
	{
		UE_LOG(LogTemp, Error, TEXT("Save file error"));
	}
}

void UMyGameInstance::OnLoadComplete(const FPlayerSaveGame& Data)
{
	ReadCoinFromFile(Data);
	ReadRPMFromFile(Data);
	ReadSetsFromFile(Data);
	ReadAnimalFromFile(Data);
	ReadPhotoFromFile(Data);
}
#pragma endregion
