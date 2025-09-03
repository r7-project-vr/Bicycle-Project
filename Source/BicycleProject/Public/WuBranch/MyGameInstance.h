// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

class UDeviceManager;
struct FQuestion;

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

	/// <summary>
	/// デバイスマネージャーを取得
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, Category = "Device")
	UDeviceManager* GetDeviceManager() const;

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
	/// コインをファイルに保存
	/// </summary>
	void SaveCoinsToFile();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateCoinDelegate, int, Num);

	/// <summary>
	/// コインの更新通知
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FUpdateCoinDelegate OnUpdateCoin;

	/// <summary>
	/// リザルトのためにクイズを保存
	/// </summary>
	/// <param name="Result"></param>
	void SaveQuizsForResult(TArray<FQuestion> Result);

	/// <summary>
	/// 一ゲームの結果を記録
	/// </summary>
	/// <param name="Result">true: クリア, false: 失敗</param>
	void SetGameResult(bool Result);

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

private:

	/// <summary>
	/// 
	/// </summary>
	void ReadCoinFromFile();

	/// <summary>
	/// コインの数を更新
	/// </summary>
	void UpdateCoin();

	UPROPERTY()
	UDeviceManager* DeviceManager;

	/// <summary>
	/// 持ってるコイン
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int Coins;

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

	/// <summary>
	/// 最大回転数
	/// </summary>
	int MaxRPM;

	/// <summary>
	/// 回転数の標準値
	/// </summary>
	int StandardRPM;
};
