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

	void EnableDefaultActions_Implementation() override;

	void DisableDefaultActions_Implementation() override;

	void EnableSelectAnswerActions_Implementation() override;

	void DisableSelectAnswerActions_Implementation() override;

private:

	/// <summary>
	/// アクションをセットアップ
	/// </summary>
	void SetupAction();

	/// <summary>
	/// MappingContextでmoveアクションに設定されたキーを押したら最初に値がもらうところ
	/// </summary>
	/// <param name="Value"></param>
	void OnMove(const FInputActionValue& Value);

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
	/// プレイヤーコントローラー
	/// </summary>
	APlayerController* _controller;

	/// <summary>
	/// デフォルトアクションマップ
	/// </summary>
	UInputMappingContext* _defaultMap;

	/// <summary>
	/// 答えを選択するアクションマップ
	/// </summary>
	UInputMappingContext* _answerSelectMap;

	/// <summary>
	/// 移動アクション
	/// </summary>
	UInputAction* _moveAction;

	/// <summary>
	/// 左の答えを選択するアクション
	/// </summary>
	UInputAction* _selectLeftAction;

	/// <summary>
	/// 右の答えを選択するアクション
	/// </summary>
	UInputAction* _selectRightAction;
};
