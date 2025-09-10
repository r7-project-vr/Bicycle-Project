// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingWidget.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API USettingWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/// <summary>
	/// 測定開始の通知
	/// </summary>
	/// <param name=""></param>
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartRecalibrationDelegate);

	FStartRecalibrationDelegate OnStartRecalibrationEvent;

};
