// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeviceType.generated.h"

/**
 * デバイスのタイプ
 */
UENUM(BlueprintType)
enum class EDeviceType: uint8
{
	Keyboard UMETA(DisplayName = "KeyBoard"),
	CustomDevice UMETA(DisplayName = "My Device"),
};

/**
 * デバイスをつける部位
 */
UENUM(BlueprintType)
enum class EDevicePart : uint8
{
	RightHand,
	LeftHand,
	RightLeg,
	LeftLeg
};

/*
* デバイスの接続状態
*/
UENUM(BlueprintType)
enum class EDeviceConnectType : uint8
{
	UnConnected,
	Connecting,
	Connected,
};