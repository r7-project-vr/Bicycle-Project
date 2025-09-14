
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WuBranch/Interface/PauseInterface.h"
#include "Animal.generated.h"

class USkeletalMeshComponent;
class UCapsuleComponent;
class UAnimalManagerComponent;

UCLASS()
class BICYCLEPROJECT_API AAnimal : public ACharacter, public IPauseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAnimal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Pause_Implementation() override;
	void ReStart_Implementation() override;
	bool IsPause_Implementation() override;

	/// <summary>
	/// 今のスピードをゲット
	/// </summary>
	/// <returns>スピード</returns>
	UFUNCTION(BlueprintCallable)
	float GetCurrentSpeed() const;

	/// <summary>
	/// 目標を設定
	/// </summary>
	/// <param name="Target">目標</param>
	/// <param name="Manager">管理者</param>
	void Init(ACharacter* Target, UAnimalManagerComponent* Manager);

	/// <summary>
	/// 偏移を変える
	/// </summary>
	/// <param name="Offset">偏移量</param>
	void ChangeOffset(FVector Offset);

private:

	enum BehaviorState
	{
		None,      // 何もしない
		Chasing,   // 目標を追う
		GivingUp,    // 諦めた
	};

	/// <summary>
	/// 行動を決定
	/// </summary>
	void DecideBehavior();

	/// <summary>
	/// 行動する
	/// </summary>
	void Action(float DeltaTime);

	/// <summary>
	/// 目標の位置をゲット
	/// </summary>
	/// <returns></returns>
	FVector GetTargetLocation();

	/// <summary>
	/// 偏移の位置をゲット
	/// </summary>
	/// <returns></returns>
	FVector GetCurrentOffsetLocation();

	/// <summary>
	/// 偏移した目標を追う
	/// </summary>
	void Chase(float DeltaTime);

	/// <summary>
	/// 移動
	/// </summary>
	/// <param name="DeltaTime"></param>
	/// <param name="Direction">方向</param>
	void Move(float DeltaTime, FVector Direction);

	/// <summary>
	/// 追うのをあきらめた
	/// </summary>
	void GiveUp();

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
	/// 新しい偏移をゲット
	/// </summary>
	/// <returns></returns>
	FVector GetNewOffset();

	/// <summary>
	/// ついている目標
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACharacter> CurrentTarget;

	/// <summary>
	/// 管理者
	/// </summary>
	UAnimalManagerComponent* AnimalManager;

	/// <summary>
	/// 行動状態
	/// </summary>
	BehaviorState CurrentState;

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
	/// スピード
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Speed;

	/// <summary>
	/// 対象との偏移
	/// </summary>
	FVector RelativeOffset;

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

	/// <summary>
	/// 停止中か
	/// </summary>
	bool IsPaused;

	/// <summary>
	/// 走るサウンド
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USoundBase* RunSE;

	/// <summary>
	/// 歩くサウンド
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USoundBase* WalkSE;

	/// <summary>
	/// 鳴くサウンド
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USoundBase* ChirpSE;
};
