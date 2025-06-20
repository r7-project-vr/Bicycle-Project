// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include "CustomDevice.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UCustomDevice : public UDevice
{
	GENERATED_BODY()

public:

	UCustomDevice();

	virtual void Init() override;

	void EnableDefaultActions_Implementation() override;

	void DisableDefaultActions_Implementation() override;

	void EnableSelectAnswerActions_Implementation() override;

	void DisableSelectAnswerActions_Implementation() override;

private:

	/// <summary>
	/// MappingContextでmoveアクションに設定されたキーを押したら最初に値がもらうところ
	/// </summary>
	/// <param name="Value"></param>
	void OnMove();

	/// <summary>
	/// MappingContextで左の答えを選ぶアクションに設定されたキーを押したら最初に値がもらうところ
	/// </summary>
	/// <param name="Value"></param>
	void OnSelectLeftAnswer();

	/// <summary>
	/// MappingContextで右の答えを選ぶアクションに設定されたキーを押したら最初に値がもらうところ
	/// </summary>
	/// <param name="Value"></param>
	void OnSelectRightAnswer();

	/// <summary>
	/// デフォルトアクションのスイッチ
	/// </summary>
	bool _defaultActionSwitch;

	/// <summary>
	/// 答えを選択するアクションのスイッチ
	/// </summary>
	bool _selectAnswerSwitch;
};
