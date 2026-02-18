// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <WuBranch/Device/DeviceType.h>
#include "BLEDeviceInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct BICYCLEPROJECT_API FBLEDeviceInfo
{
	GENERATED_BODY()

	/// <summary>
	/// デバイスの名前
	/// </summary>
	UPROPERTY(BlueprintReadOnly)
	FString Name;

	/// <summary>
	/// デバイスのUUID
	/// </summary>
	UPROPERTY(BlueprintReadOnly)
	FString UUID;

	/// <summary>
	/// 接続状態
	/// </summary>
	UPROPERTY(BlueprintReadOnly)
	EDeviceConnectType State;
	
	/// <summary>
	/// LEDの色
	/// </summary>
	FColor LEDColor[2];
};
