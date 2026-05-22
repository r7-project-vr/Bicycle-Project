// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ChoiceProvider.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAnswerSelectDelegate, float, Next);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnswerDecideDelegate);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectV2Delegate); 2026.05.11から使わない

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

	// 2026.05.11 ウー start 仕様変更
	// 本来はトリガーで直接左右の選択肢を選ぶですが、今回はスティックで選んでトリガーで決定する方式に変更したため、左右の選択肢を選ぶイベントを追加

	/// <summary>
	/// 2026.05.11から使わない
	/// 左の答えを選ぶイベントをバインドする
	/// </summary>
	/// <param name="object">バインドされたい関数があるオブジェクト</param>
	/// <param name="functionName">関数名</param>
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Choice Provider")
	//void BindSelectLeftEvent(UObject* object, FName functionName);

	/// <summary>
	/// 2026.05.11から使わない
	/// 右の答えを選ぶイベントをバインドする
	/// </summary>
	/// <param name="object">バインドされたい関数があるオブジェクト</param>
	/// <param name="functionName">関数名</param>
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Choice Provider")
	//void BindSelectRightEvent(UObject* object, FName functionName);

	/// <summary>
	/// 答えを決定するイベントをバインドする
	/// </summary>
	/// <param name="object"></param>
	/// <param name="functionName"></param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Choice Provider")
	void BindDecisionEvent(UObject* object, FName functionName);

	/// <summary>
	/// スティックで選択肢を選ぶイベントをバインドする
	/// </summary>
	/// <param name="object"></param>
	/// <param name="functionName"></param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Choice Provider")
	void BindSelectionEvent(UObject* object, FName functionName);

	/// <summary>
	/// 答えを選べるようにする
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Choice Provider")
	void EnableSelectAnswerAction();

	/// <summary>
	/// 答えを選べないようにする
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Choice Provider")
	void DisableSelectAnswerAction();

	// 2026.05.11 ウー end
};
