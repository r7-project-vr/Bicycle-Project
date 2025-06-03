// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BikePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API ABikePlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	
	/// <summary>
	/// 入力を制御する
	/// </summary>
	/// <param name="playerEnabled">true: 有効化, false:無効化</param>
	void SetPlayerEnabledState(bool playerEnabled);
};
