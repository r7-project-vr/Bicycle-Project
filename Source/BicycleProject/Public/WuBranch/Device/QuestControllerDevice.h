// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include "QuestControllerDevice.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class UQuestControllerDevice : public UDevice
{
	GENERATED_BODY()

public:

	UQuestControllerDevice();

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
	/// デバイスマッピング
	/// </summary>
	UInputMappingContext* _mappingContext;

	/// <summary>
	/// 移動アクション
	/// </summary>
	UInputAction* _moveAction;
};
