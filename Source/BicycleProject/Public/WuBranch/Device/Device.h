// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WuBranch/Interface/MoveProvider.h"
#include "WuBranch/Interface/ChoiceProvider.h"
#include "DeviceType.h"
#include "Device.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateDevice, Log, All);

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UDevice : public UObject, public IMoveProvider, public IChoiceProvider
{
	GENERATED_BODY()

public:

	UDevice();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() {};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="ID">デバイスID</param>
	/// <param name="Ver">デバイスヴァージョン</param>
	virtual void Init(int ID, int Ver) {};

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

	/// <summary>
	/// 名前をゲット
	/// </summary>
	/// <returns>デバイス名</returns>
	FString GetName() const;

	/// <summary>
	/// デバイスのUUIDをゲット
	/// </summary>
	/// <returns>UUID</returns>
	FString GetUUID() const;

	/// <summary>
	/// 接続状態をゲット
	/// </summary>
	/// <returns>接続状態</returns>
	EDeviceConnectType GetConnectState();

	void BindMoveEvent_Implementation(UObject* Object, FName FunctionName) override;

	void BindSelectLeftEvent_Implementation(UObject* Object, FName FunctionName) override;

	void BindSelectRightEvent_Implementation(UObject* Object, FName FunctionName) override;

protected:

	/// <summary>
	/// 移動イベント
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FMoveV2Delegate OnMoveEvent;

	/// <summary>
	/// 左を選ぶイベント
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FSelectV2Delegate OnSelectLeftEvent;

	/// <summary>
	/// 右を選ぶイベント
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FSelectV2Delegate OnSelectRightEvent;

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
