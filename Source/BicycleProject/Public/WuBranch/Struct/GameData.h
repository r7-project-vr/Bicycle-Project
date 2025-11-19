// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FGameData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalCoins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSoftClassPath> Animals;
};
