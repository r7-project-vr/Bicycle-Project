// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <WuBranch/Device/DeviceType.h>
#include "BLEDeviceInfo.generated.h"

class IBleDeviceInterface;

/**
 * 
 */
USTRUCT(BlueprintType)
struct BICYCLEPROJECT_API FBLEDeviceInfo
{
	GENERATED_BODY()

	/// <summary>
	/// デバイス
	/// </summary>
	TScriptInterface<IBleDeviceInterface> Device;

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
	UPROPERTY(BlueprintReadOnly)
	FColor LEDColor1;

	UPROPERTY(BlueprintReadOnly)
	FColor LEDColor2;

	bool operator == (const FBLEDeviceInfo& Other)
	{
		return this->UUID.Equals(Other.UUID);
	}
};
