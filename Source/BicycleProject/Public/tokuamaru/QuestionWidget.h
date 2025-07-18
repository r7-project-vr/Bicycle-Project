// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestionWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BICYCLEPROJECT_API UQuestionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetQuestion(const FString& content);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetAnswer1(const FString& content);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetAnswer2(const FString& content);
};
