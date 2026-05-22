// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuizUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UQuizUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/// <summary>
	/// 答えを選ぶ
	/// </summary>
	/// <param name="Operation">+1: 一個右の方を選んだ、-1：一個左の方を選んだ</param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnSelectedOption(int32 Operation);

	/// <summary>
	/// 答えを決める
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void DecideAnswer();
};
