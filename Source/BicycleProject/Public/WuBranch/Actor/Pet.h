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

	/// <summary>
	/// 一緒に走るのをやめて、その場に残る(漕ぎ過ぎの赤ゲージで振り切られた)
	/// </summary>
	void LeaveBehind();

	/// <summary>
	/// プレイヤーについて来ているか
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	bool IsFollowing() const;

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
	/// 目標の移動の速さを更新
	/// オートプレイ中は速度が0のまま位置だけ動くので、位置の変化から計算する
	/// </summary>
	void UpdateTargetSpeed(float DeltaTime);

	/// <summary>
	/// 離れすぎた(障害物に引っかかった等)時、プレイヤーの後ろに呼び戻す
	/// </summary>
	void Recall();

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
	/// 追う時の最低の速さ(cm/s)
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MinChaseSpeed;

	/// <summary>
	/// 追う時の最高の速さ(cm/s)、自転車の最高速度(約1250)より速くして置いていかれないようにする
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxChaseSpeed;

	/// <summary>
	/// 離れた距離1cmごとに足す速さ(1/s)、離れているほど速く走って追いつく
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CatchUpRate;

	/// <summary>
	/// プレイヤーの後ろに呼び戻す距離(cm)
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RecallDistance;

	/// <summary>
	/// 目標の移動の速さ(cm/s)
	/// </summary>
	float TargetSpeed;

	/// <summary>
	/// 前のフレームの目標の位置
	/// </summary>
	FVector PreviousTargetLocation;

	/// <summary>
	/// 前のフレームの目標の位置があるか
	/// </summary>
	bool bHasPreviousTargetLocation;

	/// <summary>
	/// 呼び戻しに失敗した時、次に試すまでの時間
	/// </summary>
	float RecallCoolTime;

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
