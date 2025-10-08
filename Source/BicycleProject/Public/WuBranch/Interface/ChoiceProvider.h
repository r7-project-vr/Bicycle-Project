// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ChoiceProvider.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectV2Delegate);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UChoiceProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BICYCLEPROJECT_API IChoiceProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/// <summary>
	/// 左の答えを選ぶイベントをバインドする
	/// </summary>
	/// <param name="object">バインドされたい関数があるオブジェクト</param>
	/// <param name="functionName">関数名</param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	void BindSelectLeftEvent(UObject* object, FName functionName);

	/// <summary>
	/// 右の答えを選ぶイベントをバインドする
	/// </summary>
	/// <param name="object">バインドされたい関数があるオブジェクト</param>
	/// <param name="functionName">関数名</param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	void BindSelectRightEvent(UObject* object, FName functionName);

	/// <summary>
	/// 答えを選べるようにする
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	void EnableSelectAnswerAction();

	/// <summary>
	/// 答えを選べないようにする
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	void DisableSelectAnswerAction();
};
