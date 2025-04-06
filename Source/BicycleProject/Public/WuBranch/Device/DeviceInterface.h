// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DeviceInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoveDelegate, FVector2D, dir);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectAnswerDelegate);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UDeviceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BICYCLEPROJECT_API IDeviceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/// <summary>
	/// 移動イベントをバインドする
	/// </summary>
	/// <param name="object">バインドされたい関数があるオブジェクト</param>
	/// <param name="functionName">関数名</param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	void BindMoveEvent(UObject* object, FName functionName);

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
	/// デバイスとリンクする
	/// </summary>
	/// <returns>true: 成功 / false: 失敗</returns>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	bool Connect();

	/// <summary>
	/// デバイスとのリンクを外す
	/// </summary>
	/// <returns>true: 成功 / false: 失敗</returns>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	bool Disconnect();

	/// <summary>
	/// デバイスの名前を取得
	/// </summary>
	/// <returns>デバイスの名前</returns>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	FString GetName() const;

	/// <summary>
	/// デバイスのUUIDを取得
	/// </summary>
	/// <returns>デバイスのUUID</returns>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	FString GetUUID() const;

	/// <summary>
	/// デバイスとの接続状態を取得
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	EDeviceConnectType GetConnectState();

	/// <summary>
	/// 答えを選べるようにする
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	void EnableSelectAnswerActions();

	/// <summary>
	/// 答えを選べないようにする
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	void DisableSelectAnswerActions();
};
