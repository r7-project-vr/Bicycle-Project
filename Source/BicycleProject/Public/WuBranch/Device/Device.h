// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DeviceInterface.h"
#include "Device.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateDevice, Log, All);

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UDevice : public UObject, public IDeviceInterface
{
	GENERATED_BODY()

public:

	UDevice();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() {};

	FString GetName_Implementation() const override;

	FString GetUUID_Implementation() const override;

	EDeviceConnectType GetConnectState_Implementation() const override;

	void BindMoveEvent_Implementation(UObject* object, FName functionName) override;

	void BindSelectLeftEvent_Implementation(UObject* object, FName functionName) override;

	void BindSelectRightEvent_Implementation(UObject* object, FName functionName) override;

	UPROPERTY(BlueprintAssignable)
	FMoveDelegate _onMoveEvent;

	UPROPERTY(BlueprintAssignable)
	FSelectAnswerDelegate _onSelectLeftEvent;

	UPROPERTY(BlueprintAssignable)
	FSelectAnswerDelegate _onSelectRightEvent;

protected:

	/// <summary>
	/// デバイスの名前
	/// </summary>
	FString _name;

	/// <summary>
	/// デバイスのUUID
	/// </summary>
	FString _uuid;

	/// <summary>
	/// 接続状態
	/// </summary>
	EDeviceConnectType _state;
};
