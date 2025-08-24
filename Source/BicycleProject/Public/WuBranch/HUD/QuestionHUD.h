// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "QuestionHUD.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API AQuestionHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	AQuestionHUD();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetHUDWidget() const;

	/// <summary>
	/// HUDのウィジェットをゲット
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns>ウィジェット</returns>
	template<class T>
	T* GetHUDWidget()
	{
		return Cast<T>(HUDWidget);
	}

private:

	/// <summary>
	/// HUDUIを生成して画面に追加
	/// </summary>
	void AddWidgetToView();


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> HUDUIClass;

	/// <summary>
	/// HUD Widget
	/// </summary>
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDWidget;
};
