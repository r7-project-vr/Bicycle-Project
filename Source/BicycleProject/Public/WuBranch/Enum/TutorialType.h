// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETutorialType : uint8
{
	Controller UMETA(Display = "Controller"),
	Photo UMETA(DisplayName = "Photo"),
	Coin UMETA(DisplayName = "Coin"),
	Quiz UMETA(DisplayName = "Quiz")
};
