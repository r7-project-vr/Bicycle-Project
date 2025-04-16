// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UTimerWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void SetCount();
};
