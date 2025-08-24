// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

class UDeviceManager;

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
};
