// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetRotation(float cnt_);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetRotationF(float cnt_);
	
};
