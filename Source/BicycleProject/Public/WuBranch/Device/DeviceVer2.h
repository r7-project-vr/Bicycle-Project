// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DeviceType.h"
#include "DeviceVer2.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UDeviceVer2 : public UObject
{
	GENERATED_BODY()
	
public:

	UDeviceVer2();

	/// <summary>
	/// デバイスとリンクする
	/// </summary>
	/// <returns>true: 成功 / false: 失敗</returns>
	virtual bool Connect();

	/// <summary>
	/// デバイスとのリンクを外す
	/// </summary>
	/// <returns>true: 成功 / false: 失敗</returns>
	virtual bool DisConnect();

	/// <summary>
	/// デバイスの名前を取得
	/// </summary>
	/// <returns>デバイスの名前</returns>
	UFUNCTION(BlueprintCallable, Category = "Device Interface")
	FString GetName() const;

	/// <summary>
	/// デバイスのUUIDを取得
	/// </summary>
	/// <returns>デバイスのUUID</returns>
	UFUNCTION(BlueprintCallable, Category = "Device Interface")
	FString GetUUID() const;

	/// <summary>
	/// デバイスとの接続状態を取得
	/// </summary>
	/// <returns>接続状態</returns>
	UFUNCTION(BlueprintCallable, Category = "Device Interface")
	EDeviceConnectType GetConnectState();

protected:

	/// <summary>
	/// デバイスの名前
	/// </summary>
	FString Name;

	/// <summary>
	/// デバイスのUUID
	/// </summary>
	FString UUID;

	/// <summary>
	/// 接続状態
	/// </summary>
	EDeviceConnectType State;
};
