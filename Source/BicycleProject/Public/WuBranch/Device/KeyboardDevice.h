// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include "KeyboardDevice.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UKeyboardDevice : public UDevice
{
	GENERATED_BODY()
	
public: 

	UKeyboardDevice();

	virtual void Init() override;

private:

	/// <summary>
	/// キーセットアップ
	/// </summary>
	void SetupKey();

	/// <summary>
	/// MappingContextでmoveアクションに設定されたキーを押したら最初に値がもらえるとこ
	/// </summary>
	/// <param name="Value"></param>
	void OnMove(const FInputActionValue& Value);

	/// <summary>
	/// 
	/// </summary>
	UInputMappingContext* _mappingContext;

	/// <summary>
	/// 動作
	/// </summary>
	UInputAction* _moveAction;

	/// <summary>
	/// VRヘッドセットが接続されているかどうか
	/// </summary>
	bool _isVR;
};
