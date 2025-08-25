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
	UDeviceManager();

//--------------複数の装置と繋いで全部の信号を処理する場合------------------------

	/// <summary>
	/// すべての部位のデバイスを作る
	/// </summary>
	void CreateAllDevices();

	/// <summary>
	/// すべでのデバイスとのリンクを断つ
	/// </summary>
	void DisConnectAllDevices();

	/// <summary>
	/// リストにデバイスを追加
	/// </summary>
	/// <param name="Part">デバイスをつける部位</param>
	/// <param name="Device">デバイス本体</param>
	void AddDevice(EDevicePart Part, UDevice* device);

	/// <summary>
	/// リストからデバイスを削除
	/// </summary>
	/// <param name="Part">削除したいデバイスの</param>
	void DeleteDevice(EDevicePart Part);

	/// <summary>
	/// デバイスを取得
	/// </summary>
	/// <param name="Part">取得したいデバイスのタイプ</param>
	/// <returns>デバイス</returns>
	UDevice* GetDevice(EDevicePart Part);

//-----------------------------------------------------------------------------

//--------------一つの装置のみと繋いで全部の信号を処理する場合------------------------

	/// <summary>
	/// デバイスを変更
	/// </summary>
	/// <param name="type">変更先のデバイスタイプ</param>
	void ChangeDevice(EDeviceType type);

	/// <summary>
	/// デバイスを取得
	/// </summary>
	/// <returns>デバイス</returns>
	UDevice* GetDevice();

//-----------------------------------------------------------------------------

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

private:

	/// <summary>
	/// キーボードデバイスを作る
	/// </summary>
	UDevice* CreateKeyBoardDevice();

	/// <summary>
	/// 有線デバイスを作る
	/// </summary>
	UDevice* CreateWiredDevice();

	/// <summary>
	/// 装置(複数の場合)
	/// </summary>
	UPROPERTY()
	TMap<EDevicePart, UDevice*> Devices;

	/// <summary>
	/// 装置(一つの場合)
	/// </summary>
	/// ここで必ずUPROPERTYをつける、そうでないとUObjectがUnrealのガベージコレクションに回収される
	UPROPERTY()
	UDevice* SingleDevice;

	/// <summary>
	/// 有線デバイスのID
	/// </summary>
	int WiredDeviceID = 0x03;

	/// <summary>
	/// 有線デバイスのバージョン
	/// </summary>
	int WiredDeviceVer = 0x02;
};
