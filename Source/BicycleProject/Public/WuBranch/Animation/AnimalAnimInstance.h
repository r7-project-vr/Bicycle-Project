// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimalAnimInstance.generated.h"

class AAnimal;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UAnimalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UAnimalAnimInstance();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="DeltaTime"></param>
	UFUNCTION(BlueprintCallable)
	void UpdateAnimation(float DeltaTime);

private:

	/// <summary>
	/// 動物のインスタンスを設定
	/// </summary>
	void SetAnimalInstance();

	/// <summary>
	/// 動物
	/// </summary>
	AAnimal* Animal;

	/// <summary>
	/// スピード
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Speed;
};
