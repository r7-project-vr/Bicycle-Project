﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

class UDeviceManager;
struct FQuestion;
class AAnimal;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UMyGameInstance();

	virtual void Init() override;

#pragma region デバイス
public:

	/// <summary>
	/// デバイスマネージャーを取得
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, Category = "Device")
	UDeviceManager* GetDeviceManager() const;

private:

	/// <summary>
	/// デバイスマネージャー
	/// </summary>
	UPROPERTY()
	UDeviceManager* DeviceManager;
#pragma endregion

#pragma region コイン
public:

	/// <summary>
	/// コインを取得
	/// </summary>
	/// <returns></returns>
	int GetCoins() const;

	/// <summary>
	/// コインを追加
	/// </summary>
	/// <param name="Amount">追加する量</param>
	void AddCoins(int Amount);

	/// <summary>
	/// コインの高さを取得
	/// </summary>
	/// <returns>高さ</returns>
	UFUNCTION(BlueprintCallable)
	float GetCoinHeight() const;

	/// <summary>
	/// コインの高さを設定
	/// </summary>
	/// <param name="Height">高さ</param>
	UFUNCTION(BlueprintCallable)
	void SetCoinHeight(float Height);

	/// <summary>
	/// コインの高さをリセット
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ResetCoinHeight();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateCoinDelegate, int, Num);

	/// <summary>
	/// コインの更新通知
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FUpdateCoinDelegate OnUpdateCoin;

	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateCoinHeightDelegate, float, Height);

	/// <summary>
	/// コインの高さ更新通知
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FUpdateCoinHeightDelegate OnUpdateCoinHeight;

private:

	/// <summary>
	/// コインをファイルに保存
	/// </summary>
	void SaveCoinsToFile();

	/// <summary>
	/// ファイルからコインを読み込む
	/// </summary>
	void ReadCoinFromFile();

	/// <summary>
	/// コインの数を更新
	/// </summary>
	void UpdateCoin();

	/// <summary>
	/// コインの高さを更新
	/// </summary>
	void UpdateCoinHeight();

	/// <summary>
	/// 持ってるコイン
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int Coins;

	// コインの高さ
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CoinHeight;
#pragma endregion
	
#pragma region ゲーム結果
public:

	/// <summary>
	/// リザルトのためにクイズを保存
	/// </summary>
	/// <param name="Result"></param>
	void SaveQuizsForResult(TArray<FQuestion*> Result);

	/// <summary>
	/// 一ゲームの結果を記録
	/// </summary>
	/// <param name="Result">true: クリア, false: 失敗</param>
	void SetGameResult(bool Result);

private:

	/// <summary>
	/// 答えたクイズ
	/// </summary>
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FQuestion> Quizs;

	/// <summary>
	/// ゲームクリアのフラグ
	/// </summary>
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsClear;
#pragma endregion
	
#pragma region RPM
public:

	/// <summary>
	/// 最大回転数をゲット
	/// </summary>
	/// <returns>最大回転数</returns>
	UFUNCTION(BlueprintCallable)
	int GetMaxRPM() const;

	/// <summary>
	/// 最大回転数を設定
	/// </summary>
	/// <param name="Value">回転数</param>
	UFUNCTION(BlueprintCallable)
	void SetMaxRPM(int Value);

	/// <summary>
	/// 回転数の標準値をゲット
	/// </summary>
	/// <returns>回転数の標準値</returns>
	UFUNCTION(BlueprintCallable)
	int GetStandardRPM() const;

	/// <summary>
	/// 回転数の標準値を設定
	/// </summary>
	/// <param name="Value">標準値</param>
	UFUNCTION(BlueprintCallable)
	void SetStandardRPM(int Value);

	/// <summary>
	/// 回転数の標準値をリセット
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ResetStandardRPM();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateStandardRPMDelegate, int, Num);

	/// <summary>
	/// 標準回転数が変わった通知
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FUpdateStandardRPMDelegate OnUpdateStandardRPM;

private:

	/// <summary>
	/// 標準回転数が変わったのを通知する
	/// </summary>
	void NotifyUpdateStandardRPM();

	/// <summary>
	/// 最大回転数
	/// </summary>
	int MaxRPM;

	/// <summary>
	/// 回転数の標準値
	/// </summary>
	int StandardRPM;
#pragma endregion
	
#pragma region 動物
public:

	/// <summary>
	/// 動物を追加
	/// </summary>
	/// <param name="Animal">対象動物</param>
	UFUNCTION(BlueprintCallable)
	void AddAnimal(TSubclassOf<AAnimal> Animal);

	/// <summary>
	/// 動物を削除
	/// </summary>
	/// <param name="Animal">対象動物</param>
	void RemoveAnimal(TSubclassOf<AAnimal> Animal);

	/// <summary>
	/// 全動物をゲット
	/// </summary>
	/// <returns>全動物</returns>
	TArray<TSubclassOf<AAnimal>> GetAnimals() const;

	/// <summary>
	/// ついてこれる動物の数を設定
	/// </summary>
	/// <param name="Amount">数</param>
	void SetMaxAnimalCount(int Amount);

private:
	
	/// <summary>
	/// 動物をファイルに保存
	/// </summary>
	void SaveAnimalToFile();

	/// <summary>
	/// 一ゲーム内でついてくる動物
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AAnimal>> Animals;

	/// <summary>
	/// 最大ついて来れる動物の数
	/// </summary>
	int MaxAnimalCount;
#pragma endregion

#pragma region セーブ
public:

	/// <summary>
	/// すべてのデータをセーブ
	/// </summary>
	void SaveAllToFile();

private:
	
	/// <summary>
	/// すべてのデータを読み込む
	/// </summary>
	void ReadAll();
#pragma endregion
};
