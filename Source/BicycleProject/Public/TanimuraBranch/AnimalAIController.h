// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AnimalAIController.generated.h"

/**
 * 動物AIコントローラー
 */
UCLASS()
class BICYCLEPROJECT_API AAnimalAIController : public AAIController
{
	GENERATED_BODY()

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	AAnimalAIController();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Tick(float DeltaTime) override;

protected:
	/// <summary>
	/// Pawn所持時の初期化
	/// </summary>
	virtual void OnPossess(APawn* InPawn) override;

public:
	/// <summary>
	/// 行動を決定
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void DecideBehavior();

	/// <summary>
	/// 行動を決定（BPで上書き可能）
	/// </summary>
	virtual void DecideBehavior_Implementation();

	/// <summary>
	/// 行動する
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void Action(float DeltaTime);

	/// <summary>
	/// Action（BPで上書き可）
	/// </summary>
	virtual void Action_Implementation(float DeltaTime);

protected:
	/// <summary>
	/// 目標地点への到達判定距離
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AcceptanceRadius = 100.0f;

	/// <summary>
	/// スタック判定を行う間隔
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stuck Check")
	float StuckCheckInterval = 1.0f;

	/// <summary>
	/// スタックとみなす移動距離の閾値
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stuck Check")
	float StuckThresholdDistance = 50.0f;

	/// <summary>
	/// 目的地到着後の停止時間
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WaitAfterArriveSeconds = 3.0f;

	/// <summary>
	/// 地面トレースの上方向オフセット
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Align")
	float GroundTraceUpOffset = 200.0f;

	/// <summary>
	/// 地面トレースの下方向オフセット
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Align")
	float GroundTraceDownOffset = 400.0f;

	/// <summary>
	/// 勾配に合わせた回転補間速度
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Align")
	float SlopeAlignInterpSpeed = 8.0f;

	/// <summary>
	/// 平地とみなす最小角度
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Align")
	float SlopeAlignMinAngleDeg = 3.0f;

	/// <summary>
	/// 移動速度の倍率
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MoveSpeedScale = 0.5f;

	/// <summary>
	/// 上り坂の最低速度倍率
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Speed")
	float UphillMinSpeedScale = 0.5f;

	/// <summary>
	/// 最大減速として扱う上り坂角度
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Slope Speed")
	float UphillMaxAngleDeg = 35.0f;

private:
	/// <summary>
	/// 現在の移動目標地点
	/// </summary>
	FVector CurrentTargetLocation;

	/// <summary>
	/// 目標地点が設定されているか
	/// </summary>
	bool bHasTarget;

	/// <summary>
	/// スタックチェック用タイマー
	/// </summary>
	float StuckCheckTimer;

	/// <summary>
	/// 前回のチェック時の位置
	/// </summary>
	FVector LastCheckLocation;

	/// <summary>
	/// 待機中かどうか
	/// </summary>
	bool bIsWaiting;

	/// <summary>
	/// 待機用タイマー
	/// </summary>
	float WaitTimer;

	/// <summary>
	/// 現在の地面法線
	/// </summary>
	FVector CurrentGroundNormal = FVector::UpVector;

	/// <summary>
	/// 地面法線が有効か
	/// </summary>
	bool bHasGroundNormal = false;

	/// <summary>
	/// 勾配に合わせて向きを補正
	/// </summary>
	void UpdateSlopeAlignment(float DeltaTime, const FVector& MoveDirection);

	/// <summary>
	/// 上り坂時の速度倍率を計算
	/// </summary>
	float ComputeSlopeSpeedScale(const FVector& MoveDirection) const;
};
