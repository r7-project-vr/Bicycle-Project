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
	/// デフォルトアクションを機能させる
	/// </summary>
	void EnableDefaultActions();

	/// <summary>
	/// デフォルトアクションを機能させない
	/// </summary>
	void DisableDefaultActions();

	/// <summary>
	/// 答えを選べるようにする
	/// </summary>
	void EnableSelectAnswerActions();

	/// <summary>
	/// 答えを選べないようにする
	/// </summary>
	void DisableSelectAnswerActions();

	/// <summary>
	/// 移動イベントをバインド
	/// </summary>
	/// <param name="object">バインドされたい関数があるオブジェクト</param>
	/// <param name="functionName">関数名</param>
	UFUNCTION()
	void BindMoveEvent(UObject* object, FName functionName);

	/// <summary>
	/// 左の答えを選ぶイベントをバインドする
	/// </summary>
	/// <param name="object">バインドされたい関数があるオブジェクト</param>
	/// <param name="functionName">関数名</param>
	UFUNCTION()
	void BindSelectLeftEvent(UObject* object, FName functionName);

	/// <summary>
	/// 左の答えを選ぶイベントをバインドする
	/// </summary>
	/// <param name="object">バインドされたい関数があるオブジェクト</param>
	/// <param name="functionName">関数名</param>
	UFUNCTION()
	void BindSelectRightEvent(UObject* object, FName functionName);

	/// <summary>
	/// デバイスを取得
	/// </summary>
	/// <returns>デバイス</returns>
	UDevice* GetDevice();

//-----------------------------------------------------------------------------
private:

	/// <summary>
	/// unrealがサポートしているデバイスを作る
	/// </summary>
	void CreateKeyBoardDevice();

	/// <summary>
	/// 自作デバイスを作る
	/// </summary>
	void CreateCustomDevice();

	/// <summary>
	/// 装置(複数の場合)
	/// </summary>
	TMap<EDevicePart, TScriptInterface<IDeviceInterface>> _devices;

	/// <summary>
	/// 装置(一つの場合)
	/// </summary>
	/// ここで必ずUPROPERTYをつける、そうでないとUObjectがUnrealのガベージコレクションに回収される
	UPROPERTY()
	UDevice* _device;
};
