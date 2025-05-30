// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionUIWidget.generated.h"

/**
 * 
 */
struct FQuestion;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOptionSelected, int32, SelectedOptionIndex);
UCLASS(Abstract, Blueprintable)
class BICYCLEPROJECT_API UOptionUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetQuestionAndAnswer(const FQuestion& Question);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnOptionSelected OnOptionSelected;

	void OnSelectedOption(int32 OptionIndex);

	int GetQuestionID()const;
protected:

	UFUNCTION(BlueprintImplementableEvent)
	void SetQuestion(const FString& quest);



	UFUNCTION(BlueprintImplementableEvent)
	void SetAnswer(int32 answerNum, const TArray<FString>& answers);

private:
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	int32 questionID;
	int32 MaxPressTime=3.f;

	void GetQuestionID(int32 questionID);
};
