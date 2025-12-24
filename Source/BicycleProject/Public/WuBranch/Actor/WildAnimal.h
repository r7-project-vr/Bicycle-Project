// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Actor/Animal.h"
#include "WildAnimal.generated.h"
class AAIController;
/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API AWildAnimal : public AAnimal
{
	GENERATED_BODY()
	
public:

	AWildAnimal();

	virtual void Init(ACharacter* Target, UAnimalManagerComponent* Manager, UAnimalDataAsset* Data) override;

protected:

	virtual void BeginPlay() override;

	virtual void DecideBehavior() override;

	virtual void Action(float DeltaTime) override;

private:

	/// <summary>
	/// 目標が通り過ぎたか
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	bool IsTargetPassed();

	/// <summary>
	/// ターゲットを注視
	/// </summary>
	/// <param name="DeltaTime"></param>
	void StareAtTarget(float DeltaTime);

	/// <summary>
	/// 前方
	/// </summary>
	FVector Face;

	/// <summary>
	/// 前回の外積結果
	/// </summary>
	FVector PreviousCross;
	
	/// <summary>
	/// 鳴くサウンド
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USoundBase* ChirpSE;

	/// <summary>
	/// 鳴くか
	/// </summary>
	bool NeedChirp;

	/// <summary>
	/// 回転スピード
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RotationSpeed;

	/// <summary>
	/// 野生動物のAIコントローラ
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AAIController> WildAnimalControllerClass;
};
