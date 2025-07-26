// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include "WiredDevice.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UWiredDevice : public UDevice
{
	GENERATED_BODY()
	
public:

	UWiredDevice();

	virtual void Init() override;

	virtual bool Connect() override;

	virtual bool Disconnect() override;

	void EnableMoveAction_Implementation() override;

	void DisableMoveAction_Implementation() override;

	void EnableSelectAnswerAction_Implementation() override;

	void DisableSelectAnswerAction_Implementation() override;

private:

	/// <summary>
	/// デバイスを探す
	/// </summary>
	void FindDevice();

	/// <summary>
	/// 移動機能のスイッチ
	/// </summary>
	bool MoveSwitch;

	/// <summary>
	/// 選ぶ機能のスイッチ
	/// </summary>
	bool SelectSwitch;
};
