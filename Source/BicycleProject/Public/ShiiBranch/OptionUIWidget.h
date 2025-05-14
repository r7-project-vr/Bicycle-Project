// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionUIWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BICYCLEPROJECT_API UOptionUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetQuestionAndAnswer();

protected:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetQuestion(const FString& quest);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetAnswer(const FString& answer);

};
