// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WuBranch/Struct/TutorialData.h"
#include "TutorialMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTutorialChooseCompleted);

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UTutorialMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/// <summary>
	/// 画面内容を構築する
	/// </summary>
	/// <param name="Datas"></param>
	UFUNCTION(BlueprintImplementableEvent)
	void SetMenu(const TArray<FTutorialData>& Datas);

	/// <summary>
	/// 画面を表示する
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void ShowScene();

	/// <summary>
	/// 画面を隠す
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CloseScene();

	/// <summary>
	/// チュートリアルを選び終わった通知
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FTutorialChooseCompleted TutorialChooseCompletedDelegate;

protected:

	/// <summary>
	/// チュートリアルを選び終わったことを通知する
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void NotifyHaveChoosed();
};
