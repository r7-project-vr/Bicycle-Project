
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

	/// <summary>
	/// IDをゲット
	/// </summary>
	/// <returns>ID</returns>
	int32 GetMyID() const;

protected:

	enum BehaviorState
	{
		None,      // 何もしない
		Chasing,   // 目標を追う
		GivingUp,    // 諦めた
	};

	/// <summary>
	/// 行動を決定
	/// </summary>
	virtual void DecideBehavior();

	/// <summary>
	/// 行動する
	/// </summary>
	virtual void Action(float DeltaTime);

	/// <summary>
	/// 目標の位置をゲット
	/// </summary>
	/// <returns></returns>
	FVector GetMyTargetLocation();

	/// <summary>
	/// 偏移の位置をゲット
	/// </summary>
	/// <returns></returns>
	FVector GetOffsetWorldLocation();

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
	/// ついている目標
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACharacter> CurrentTarget;

	/// <summary>
	/// 管理者
	/// </summary>
	UAnimalManagerComponent* AnimalManager;

	/// <summary>
	/// ID
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 ID;

	/// <summary>
	/// 行動状態
	/// </summary>
	BehaviorState CurrentState;

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
	/// 停止中か
	/// </summary>
	bool IsPaused;

};
