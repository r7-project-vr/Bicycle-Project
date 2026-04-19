// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PhotoProvider.generated.h"

// Screenshot Event
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenshotEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotoReadyEvent);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPhotoProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BICYCLEPROJECT_API IPhotoProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/// <summary>
	/// 写真を撮るイベントを登録
	/// </summary>
	/// <param name="object"></param>
	/// <param name="functionName"></param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Photo Provider")
	void BindTakePhotoEvent(UObject* object, FName functionName);

	/// <summary>
	/// 起動する
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Photo Provider")
	void EnableTakePhotoAction();

	/// <summary>
	/// 停止する
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Photo Provider")
	void DisableTakePhotoAction();

	/// <summary>
	/// 写真を撮る準備イベントを登録
	/// </summary>
	/// <param name="object"></param>
	/// <param name="functionName"></param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Photo Provider")
	void BindPhotoReadyEvent(UObject* object, FName functionName);

};
