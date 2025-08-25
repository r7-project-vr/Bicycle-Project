// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WuBranch/QuestionGameMode.h"
#include "WuBranch/Bike/BikeComponent.h"
#include <tokuamaru/ResultWidget.h>
#include <Components/WidgetComponent.h>
#include "ResultSceneComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UResultSceneComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UResultSceneComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 2025.08.25 ウー start
	/// <summary>
	/// 正解数と不正解数を更新する
	/// </summary>
	/// <param name="Correct">正解数</param>
	/// <param name="Wrong">不正解数</param>
	UFUNCTION()
	void UpdateAnswer(int Correct, int Wrong);

	//float answerTrue;
	//float answerFalse;
	
	/// <summary>
	/// オートプレイのUI更新
	/// </summary>
	/// <param name="IsAutoPlay">オートプレイ</param>
	UFUNCTION()
	void UpdateAutoPlay(bool IsAutoPlay);

	// 2025.08.25 ウー end
	UPROPERTY()
	UWidgetComponent* widgetComp;


	UPROPERTY()
	UUserWidget* widget;


	UPROPERTY()
	UResultWidget* resultWidget;


	//UPROPERTY()
	//TArray<USceneComponent*> Children;


	UPROPERTY()
	AQuestionGameMode* gamemode;


	UPROPERTY()
	UBikeComponent* bikeComponent;

	// 2025.08.25 ウー start
	//bool autoplay;
	// 2025.08.25 ウー end
	//	
};
