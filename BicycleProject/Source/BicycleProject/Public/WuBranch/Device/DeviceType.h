// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeviceType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EDeviceType: uint8
{
	RightHand,
	LeftHand,
	RightLeg,
	LeftLeg
};
