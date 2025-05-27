// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionUIWidget.generated.h"

/**
 * 
 */
struct FQuestion;
UCLASS(Abstract, Blueprintable)
class BICYCLEPROJECT_API UOptionUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetQuestionAndAnswer(const FQuestion& Question);

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void SetQuestion(const FString& quest);

	UFUNCTION(BlueprintImplementableEvent)
	void SetAnswer(int32 answerNum, const TArray<FString>& answers);

private:
	int32 questionID;

	void GetQuestionID(int32 questionID);
};
