// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETutorialType : uint8
{
	Photo UMETA(DisplayName = "Photo"),
	Coin UMETA(DisplayName = "Coin"),
	SpeedOver UMETA(DisplayName = "SpeedOver"),
	Quiz UMETA(DisplayName = "Quiz")
};
