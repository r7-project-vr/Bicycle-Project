// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AnimalAIController.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API AAnimalAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAnimalAIController();

	// AActor interface
	virtual void Tick(float DeltaTime) override;
	// End of AActor interface

protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	/// <summary>
	/// 行動を決定
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void DecideBehavior();
	virtual void DecideBehavior_Implementation();

	/// <summary>
	/// 行動する
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void Action(float DeltaTime);
	virtual void Action_Implementation(float DeltaTime);

protected:
	// 目標地点への到達判定距離
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AcceptanceRadius = 100.0f;

	// スタック判定を行う間隔（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stuck Check")
	float StuckCheckInterval = 1.0f;

	// スタックとみなす移動距離の閾値
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stuck Check")
	float StuckThresholdDistance = 50.0f;

	// 到着後の停止時間（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WaitAfterArriveSeconds = 3.0f;

	// 地面トレースの上方向オフセット
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Align")
	float GroundTraceUpOffset = 200.0f;

	// 地面トレースの下方向オフセット
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Align")
	float GroundTraceDownOffset = 400.0f;

	// 勾配に合わせる回転補間速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Align")
	float SlopeAlignInterpSpeed = 8.0f;

	// 平地とみなす最小角度（度）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Align")
	float SlopeAlignMinAngleDeg = 3.0f;

	// 移動速度スケール（1.0=通常、0.5=半分）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MoveSpeedScale = 0.5f;

	// 上り坂の最低速度スケール（0.3なら最大で30%まで減速）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Speed")
	float UphillMinSpeedScale = 0.5f;

	// この角度以上は最大減速として扱う（度）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Speed")
	float UphillMaxAngleDeg = 35.0f;

private:
	// 現在の移動目標地点
	FVector CurrentTargetLocation;

	// 目標地点が設定されているか
	bool bHasTarget;

	// スタックチェック用タイマー
	float StuckCheckTimer;

	// 前回のチェック時の位置
	FVector LastCheckLocation;

	// 待機中かどうか
	bool bIsWaiting;

	// 待機用タイマー
	float WaitTimer;

	// 現在の地面法線（速度計算用）
	FVector CurrentGroundNormal = FVector::UpVector;

	// 地面法線が有効か
	bool bHasGroundNormal = false;

	// 勾配に合わせて向きを補正
	void UpdateSlopeAlignment(float DeltaTime, const FVector& MoveDirection);

	// 上り坂時の速度スケールを計算
	float ComputeSlopeSpeedScale(const FVector& MoveDirection) const;
};
