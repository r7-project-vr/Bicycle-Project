// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DeviceInterface.h"
#include "DeviceType.h"
#include "DeviceManager.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UDeviceManager : public UObject
{
	GENERATED_BODY()
	
public:

	/// <summary>
	/// リストにデバイスを追加
	/// </summary>
	/// <param name="type">デバイスのタイプ</param>
	/// <param name="device">デバイス本体</param>
	void AddDevice(EDeviceType type, TScriptInterface<IDeviceInterface> device);

	/// <summary>
	/// リストからデバイスを削除
	/// </summary>
	/// <param name="type">削除したいデバイスのタイプ</param>
	void DeleteDevice(EDeviceType type);

	/// <summary>
	/// デバイスを取得
	/// </summary>
	/// <param name="type">取得したいデバイスのタイプ</param>
	/// <returns>デバイス</returns>
	IDeviceInterface* GetDevice(EDeviceType type);

private:

	/// <summary>
	/// 装置
	/// </summary>
	TMap<EDeviceType, TScriptInterface<IDeviceInterface>> devices;
};
