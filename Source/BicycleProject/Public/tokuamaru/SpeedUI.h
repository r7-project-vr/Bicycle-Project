// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpeedUI.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API USpeedUI : public UUserWidget
{
	GENERATED_BODY()
public:

	/// <summary>
	/// 受け取る値が自転車の速度（RPM）のときの速度取得用関数
	/// </summary>
	/// <param name="bikeSpeed"></param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CounterChange(float bikeSpeed);

	/// <summary>
	/// 受け取る値が直に自転車の速度の割合だった時の速度取得用関数
	/// </summary>
	/// <param name="bikeSpeed"></param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CounterChangeAninx(float bikeSpeed);

	/// <summary>
	/// 値の初期化。
	/// </summary>
	/// <param name="init"></param>
	/// <param name="max"></param>
	/// <param name="danger"></param>
	/// <param name="safe"></param>
	/// <param name="avalage"></param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Init(float init, float max, float danger, float safe, float avalage);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SubTherd(float delta);

	/// <summary>
	/// UIの針を動かすための数値
	/// </summary>
	UPROPERTY(BlueprintReadWrite)
	float currentY = 0.0f;

	/// <summary>
	/// 最大速度。dangerSpeedと同じ
	/// </summary>
	UPROPERTY(BlueprintReadWrite)
	float maxSpeed = 10.0f;

	/// <summary>
	/// 現在の速度
	/// </summary>
	UPROPERTY(BlueprintReadWrite)
	float nowSpeed = 0.0f;

	/// <summary>
	///危険速度の値。(RPM)
	/// </summary>
	UPROPERTY(BlueprintReadWrite)
	float dangerSpeed = 0.0f;
	
	/// <summary>
	/// 適切速度の値(RPM)
	/// </summary>
	UPROPERTY(BlueprintReadWrite)
	float safeSpeed = 0.0f;

	/// <summary>
	/// 基準速度。
	/// </summary>
	UPROPERTY(BlueprintReadWrite)
	float avalageSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	bool penalty = false;




	
};
