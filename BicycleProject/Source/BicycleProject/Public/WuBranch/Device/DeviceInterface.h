// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DeviceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
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
	/// 値を取得
	/// </summary>
	/// <returns>値</returns>
	virtual float GetValue();

	/// <summary>
	/// デバイスとリンクする
	/// </summary>
	/// <returns>true: 成功 / false: 失敗</returns>
	virtual bool Connect();

	/// <summary>
	/// デバイスとのリンクを外す
	/// </summary>
	/// <returns>true: 成功 / false: 失敗</returns>
	virtual bool Disconnect();
};
