// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AnimalAIController.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API AAnimalAIController : public AAIController
{
	GENERATED_BODY()

public:
	/// <summary>
	/// 行動を決定
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void DecideBehavior();

	/// <summary>
	/// 行動する
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void Action(float DeltaTime);
};
