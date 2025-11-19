// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Device/DeviceManager.h"
#include <UntakuBranch/Question.h>
#include "WuBranch/Actor/Animal.h"
#include "WuBranch/System/FileOperator.h"
#include "WuBranch/Struct/GameData.h"

UMyGameInstance::UMyGameInstance()
	: TotalCoins(0)
	, IsClear(false)
	, StandardRPM(50)
	, RPMThreshold(10)
	, MaxStandardRPM(60)
	// 2025.11.09 谷村 start
	, NumOfSets(4)
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

void UMyGameInstance::PrepareCoinFileData(FGameData& oData)
{
	oData.TotalCoins = TotalCoins;
}

void UMyGameInstance::InitializeCoin()
{
	TotalCoins = 0; // 初期化
	CoinHeight = 100.f;
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
#pragma endregion
// 2025.11.12 谷村 end

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

void UMyGameInstance::PrepareAnimalFileData(FGameData& oData)
{
	for (TSubclassOf<AAnimal> AnimalClass : Animals)
	{
		oData.Animals.Add(FSoftClassPath(AnimalClass));
	}
}
void UMyGameInstance::InitializeAnimal()
{
	Animals.Empty();
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
	FString Path = FileOperator::GetInstance().GetFullPath(FileName);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Path);

	// セーブデータ作成
	FGameData SaveData;
	PrepareAnimalFileData(SaveData);
	PrepareCoinFileData(SaveData);

	// JSONに変換
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetNumberField(CoinKey, SaveData.TotalCoins);
	TArray<TSharedPtr<FJsonValue>> AnimalArray;
	for (const FSoftClassPath& AnimalPath : SaveData.Animals)
	{
		AnimalArray.Add(MakeShareable(new FJsonValueString(AnimalPath.ToString())));
	}
	JsonObject->SetArrayField(AnimalKey, AnimalArray);

	// JSONを文字列に変換
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
	{
		// ファイルに書き込み
		if (FileOperator::GetInstance().WriteFile(Path, OutputString))
		{
			UE_LOG(LogTemp, Log, TEXT("セーブ成功"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("セーブ失敗: ファイル書き込みエラー"));
		}
	}
}

void UMyGameInstance::ReadAll()
{
	FString Data;
	if (FileOperator::GetInstance().ReadFile(FileName, Data))
	{
		// 文字列をJSONに変換
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Data);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			// データを読み込み
			TotalCoins = JsonObject->GetIntegerField(CoinKey);
			Animals.Empty();
			const TArray<TSharedPtr<FJsonValue>>* AnimalArray;
			if (JsonObject->TryGetArrayField(AnimalKey, AnimalArray))
			{
				for (const TSharedPtr<FJsonValue>& Value : *AnimalArray)
				{
					FString AnimalPathString = Value->AsString();
					FSoftClassPath AnimalPath(AnimalPathString);
					UClass* AnimalClass = AnimalPath.TryLoadClass<AAnimal>();
					if (AnimalClass)
					{
						Animals.Add(AnimalClass);
					}
				}
			}
			return; // 正常終了
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("セーブデータの読み込み失敗: JSON解析エラー"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("セーブデータが存在しません。初期値を使用します。"));
		InitializeCoin();
		InitializeAnimal();
	}
}
#pragma endregion
