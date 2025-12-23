// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Actor/Animal.h"
#include "Pet.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API APet : public AAnimal
{
	GENERATED_BODY()

public:

	APet();
	
	virtual void Init(ACharacter* Target, UAnimalManagerComponent* Manager, UAnimalDataAsset* Data) override;

protected:

	virtual void BeginPlay() override;

	virtual void DecideBehavior() override;

	virtual void Action(float DeltaTime) override;

private:

	/// <summary>
	/// 偏移した目標を追う
	/// </summary>
	void Chase(float DeltaTime);

	/// <summary>
	/// 新しい偏移をゲット
	/// </summary>
	/// <returns></returns>
	FVector GetNewOffset();

	/// <summary>
	/// 鳴くサウンドを再生
	/// </summary>
	void PlayChirp();

	/// <summary>
	/// 偏移を変えるカウントダウン
	/// </summary>
	/// <param name="DeltaTime"></param>
	//void CountDownChangeOffset(float DeltaTime);

	/// <summary>
	/// 次の変更時間を更新
	/// </summary>
	/// <returns>時間</returns>
	//float GetNextChangeOffsetTime();

	/// <summary>
	/// 目標を追うか
	/// </summary>
	bool IsChaseTarget;

	/// <summary>
	/// 追う位置
	/// </summary>
	FVector ChaseLocation;

	/// <summary>
	/// ターゲットを追う割合、0～0.99
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float TargetChaseRate;

	/// <summary>
	/// 追い始まる距離
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float StartChaseDistance;

	/// <summary>
	/// 追わなくなる距離
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float GiveUpDistance;

	/// <summary>
	/// 鳴くサウンド
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USoundBase* ChirpSE;

	/// <summary>
	/// すでに鳴いたか
	/// </summary>
	bool IsPlayedChirp;

	/// <summary>
	/// 偏移量を変える経過時間
	/// </summary>
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//float ChangeOffsetTime;

	/// <summary>
	/// ランダムの偏差
	/// </summary>
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//float RandomDeviation;

	/// <summary>
	/// 時間カウント
	/// </summary>
	//float TimeCnt;
};
