// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UTutorialIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/// <summary>
	/// チュートリアルアイコンを表示する
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void ShowTutorialIcon();

	/// <summary>
	/// チュートリアルアイコンを非表示する
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void HideTutorialIcon();
};
