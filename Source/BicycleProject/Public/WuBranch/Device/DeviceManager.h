// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DeviceInterface.h"
#include "DeviceType.h"
#include "UObject/ScriptDelegates.h"
#include "DeviceManager.generated.h"

class UDevice;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UDeviceManager : public UObject
{
	GENERATED_BODY()
	
public:

//--------------複数の装置と繋いで全部の信号を処理する場合------------------------

	/// <summary>
	/// リストにデバイスを追加
	/// </summary>
	/// <param name="part">デバイスをつける部位</param>
	/// <param name="device">デバイス本体</param>
	void AddDevice(EDevicePart part, TScriptInterface<IDeviceInterface> device);

	/// <summary>
	/// リストからデバイスを削除
	/// </summary>
	/// <param name="part">削除したいデバイスの</param>
	void DeleteDevice(EDevicePart part);

	/// <summary>
	/// デバイスを取得
	/// </summary>
	/// <param name="part">取得したいデバイスのタイプ</param>
	/// <returns>デバイス</returns>
	IDeviceInterface* GetDevice(EDevicePart part);

//-----------------------------------------------------------------------------

//--------------一つの装置のみと繋いで全部の信号を処理する場合------------------------

	/// <summary>
	/// デバイスを変更
	/// </summary>
	/// <param name="type">変更先のデバイスタイプ</param>
	void ChangeDevice(EDeviceType type);

	/// <summary>
	/// 移動イベントを取得
	/// </summary>
	/// <returns>移動イベント</returns>
	UFUNCTION()
	void BindMoveEvent(UObject* object, FName functionName);

	/// <summary>
	/// デバイスを取得
	/// </summary>
	/// <returns>デバイス</returns>
	UDevice* GetDevice();

//-----------------------------------------------------------------------------
private:

	/// <summary>
	/// キーボードデバイスを作る
	/// </summary>
	void CreateKeyBoardDevice();

	/// <summary>
	/// 装置(複数の場合)
	/// </summary>
	TMap<EDevicePart, TScriptInterface<IDeviceInterface>> _devices;

	/// <summary>
	/// 装置(一つの場合)
	/// </summary>
	UDevice* _device;
};
