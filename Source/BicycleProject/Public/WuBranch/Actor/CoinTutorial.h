// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Actor/TutorialActor.h"
#include "WuBranch/Interface/CoinMakerInterface.h"
#include "CoinTutorial.generated.h"

class ACoin;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API ACoinTutorial : public ATutorialActor , public ICoinMakerInterface
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:

	virtual void HandleCoinCollected(ACoin* TouchedCoin) override;

protected:

	/// <summary>
	/// コインを配置する
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void PlaceCoin();

	UFUNCTION(BlueprintCallable)
	void HideCoin();

private:

	/// <summary>
	/// プレイヤーを探す
	/// </summary>
	void FindPlayer();

	/// <summary>
	/// コインを生成する
	/// </summary>
	void SpawnCoin();

	/// <summary>
	/// コインの位置と方向を取得する
	/// </summary>
	/// <param name="OLocation"></param>
	/// <param name="ODir"></param>
	void GetCoinLocationAndDir(FVector& OLocation, FRotator& ODir);

	/// <summary>
	/// コインを表示する
	/// </summary>
	void ShowCoin();

	/// <summary>
	/// コインのテンプレート
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACoin> CoinTemplate;

	/// <summary>
	/// チュートリアル用のコイン
	/// </summary>
	UPROPERTY()
	ACoin* Coin;

	/// <summary>
	/// プレイヤー
	/// </summary>
	UPROPERTY()
	ACharacter* Player;

	/// <summary>
	/// プレイヤーとの距離
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float CoinDistanceFromPlayer;
};
