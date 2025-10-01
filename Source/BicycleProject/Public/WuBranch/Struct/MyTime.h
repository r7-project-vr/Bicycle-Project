// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyTime.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMyTime
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 hour24;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 min;
};
