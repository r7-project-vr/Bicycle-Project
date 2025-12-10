// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include "KeyboardDevice.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * キーボードの入力なので、プラットフォームがウィンドウズの場合、既に繋がっている状態がデフォルトとして認識します
 * 他のプラットフォームだったら、有線か無線で繋がっているかどうかの判断が必要です
 */
UCLASS()
class BICYCLEPROJECT_API UKeyboardDevice : public UDevice
{
	GENERATED_BODY()
	
public: 

	UKeyboardDevice();

	virtual void Init() override;

	virtual bool Connect() override;

	virtual bool Disconnect() override;

	void EnableMoveAction_Implementation() override;

	void DisableMoveAction_Implementation() override;

	void EnableSelectAnswerAction_Implementation() override;

	void DisableSelectAnswerAction_Implementation() override;

	// 注意: OnMoveEvent, OnSelectLeftEvent, OnSelectRightEvent は親クラスUDeviceで既に定義されているため、
	// ここでは宣言しません

	// Screenshot Event（新規追加のみ）
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenshotEvent);

	UPROPERTY(BlueprintAssignable, Category = "Input Events")
	FOnScreenshotEvent OnScreenshotEvent;

public:
	void OnScreenshot();

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
	APlayerController* Controller;

	/// <summary>
	/// デフォルトアクションマップ
	/// </summary>
	UInputMappingContext* DefualtMap;

	/// <summary>
	/// 答えを選択するアクションマップ
	/// </summary>
	UInputMappingContext* AnswerSelectMap;

	/// <summary>
	/// 移動アクション
	/// </summary>
	UInputAction* MoveAction;

	/// <summary>
	/// 左の答えを選択するアクション
	/// </summary>
	UInputAction* SelectLeftAction;

	/// <summary>
	/// 右の答えを選択するアクション
	/// </summary>
	UInputAction* SelectRightAction;

	/// <summary>
	/// スクリーンショットアクション
	/// </summary>
	UPROPERTY()
	UInputAction* ScreenshotAction;
};
