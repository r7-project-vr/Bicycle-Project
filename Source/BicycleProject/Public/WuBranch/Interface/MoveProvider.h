// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MoveProvider.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoveV2Delegate, FVector2D, dir);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMoveProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BICYCLEPROJECT_API IMoveProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/// <summary>
	/// 移動イベントを登録
	/// </summary>
	/// <param name="object"></param>
	/// <param name="functionName"></param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Move Provider")
	void BindMoveEvent(UObject* object, FName functionName);

	/// <summary>
	/// 起動する
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Move Provider")
	void EnableMove();

	/// <summary>
	/// 停止する
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Move Provider")
	void DisableMove();
};
