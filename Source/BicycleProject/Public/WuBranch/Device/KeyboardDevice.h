// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/Device.h"
#include "WuBranch/Interface/PhotoProvider.h"
#include "KeyboardDevice.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * キーボードの入力なので、プラットフォームがウィンドウズの場合、既に繋がっている状態がデフォルトとして認識します
 * 他のプラットフォームだったら、有線か無線で繋がっているかどうかの判断が必要です
 */
UCLASS()
class BICYCLEPROJECT_API UKeyboardDevice : public UDevice, public FTickableGameObject, public IPhotoProvider
{
	GENERATED_BODY()
	
public: 

	UKeyboardDevice();

#pragma region TickableGameObject
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	virtual bool IsTickableInEditor() const override;
#pragma endregion

	virtual void Init() override;

	void Enable() override;

	virtual bool Connect() override;

	virtual bool Disconnect() override;

	void EnableMoveAction_Implementation() override;

	void DisableMoveAction_Implementation() override;

	void EnableSelectAnswerAction_Implementation() override;

	void DisableSelectAnswerAction_Implementation() override;

	void BindTakePhotoEvent_Implementation(UObject* Object, FName FunctionName) override;

	void EnableTakePhotoAction_Implementation() override;

	void DisableTakePhotoAction_Implementation() override;

	void BindPhotoReadyEvent_Implementation(UObject* Object, FName FunctionName) override;

protected:

	UPROPERTY(BlueprintAssignable, Category = "Input Events")
	FOnScreenshotEvent OnScreenshotEvent;

	UPROPERTY(BlueprintAssignable, Category = "Input Events")
	FOnPhotoReadyEvent OnPhotoReadyEvent;

private:

	/// <summary>
	/// アクションをセットアップ
	/// </summary>
	void SetupAction();

	/// <summary>
	/// MappingContextでmoveアクションに設定されたキーを押したら最初に値がもらうところ
	/// </summary>
	/// <param name="Value"></param>
	UFUNCTION()
	void OnMove(const FInputActionValue& Value);

	/// <summary>
	/// MappingContextで左の答えを選ぶアクションに設定されたキーを押したら最初に値がもらうところ
	/// </summary>
	/// <param name="Value"></param>
	UFUNCTION()
	void OnSelectLeftAnswer();

	/// <summary>
	/// MappingContextで右の答えを選ぶアクションに設定されたキーを押したら最初に値がもらうところ
	/// </summary>
	/// <param name="Value"></param>
	UFUNCTION()
	void OnSelectRightAnswer();

	/// <summary>
	/// 写真を撮る
	/// </summary>
	UFUNCTION()
	void OnScreenshot();

	/// <summary>
	/// 写真を撮る準備
	/// </summary>
	UFUNCTION()
	void OnPhotoReady();

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
	/// 写真を撮るアクションマップ
	/// </summary>
	UInputMappingContext* PhotoMap;

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

	/// <summary>
	/// 写真を撮る準備アクション
	/// </summary>
	UInputAction* PhotoReadyAction;

	/// <summary>
	/// 自作デバイスの入力をシミュレートする間隔
	/// </summary>
	const float SimulateCustomDeviceInputInterval = 0.5f;

	/// <summary>
	/// 時間カウンター
	/// </summary>
	float TimeCnt;

	/// <summary>
	/// 自作デバイスの入力をシミュレートするためのフラグ
	/// </summary>
	bool bCanSendMoveInput;
};
