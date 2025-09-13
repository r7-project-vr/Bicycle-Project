// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HeightTesterComponent.generated.h"

class UMyGameInstance;
class USphereComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UHeightTesterComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHeightTesterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// 測定開始
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void StartRecalibration();

	/// <summary>
	/// 使う手を設定する, true: 使う, false: 使わない
	/// </summary>
	/// <param name="Left">左手</param>
	/// <param name="Right">右手</param>
	void SetUseHand(bool Left, bool Right);

	/// <summary>
	/// 使う手をゲット, true: 使う, false: 使わない
	/// </summary>
	/// <param name="OutLeft">左手</param>
	/// <param name="OutRight">右手</param>
	void GetUsedHand(bool& OutLeft, bool& OutRight);

	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRecalibrationCompletedDelegate);

	/// <summary>
	/// 測定終了の通知
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FRecalibrationCompletedDelegate OnRecalibrationCompleted;

private:

	/// <summary>
	/// 一回の測定
	/// </summary>
	UFUNCTION()
	void DoOneRecalibration();

	/// <summary>
	/// 測定終了
	/// </summary>
	void FinishRecalibration();

	/// <summary>
	/// 平均計算
	/// </summary>
	/// <returns>平均値</returns>
	float CaculateAvg();

	/// <summary>
	/// ゲーム内で新しいコインの高さを使用
	/// </summary>
	/// <param name="Avg"></param>
	void UseResultInGame(float Avg);

	/// <summary>
	/// 測定終了を通知
	/// </summary>
	void NotifyRecalibrationCompleted();

	/// <summary>
	/// ゲームインスタンス
	/// </summary>
	UMyGameInstance* GameInstance;

	/// <summary>
	/// 生成物
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* Product;

	/// <summary>
	/// 右手
	/// </summary>
	USphereComponent* RightHand;

	/// <summary>
	/// 右手を使う
	/// </summary>
	bool IsUseRight;

	/// <summary>
	/// 左手
	/// </summary>
	USphereComponent* LeftHand;

	/// <summary>
	/// 左手を使う
	/// </summary>
	bool IsUseLeft;

	/// <summary>
	/// 測定回数
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int TotalRecalibrationCount;

	/// <summary>
	/// 測定時間
	/// </summary>
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//float RecalibrationTime;

	/// <summary>
	/// サンプルを採取間隔
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CollectionInterval;

	/// <summary>
	/// 毎回の測定結果
	/// </summary>
	TArray<float> Results;

	/// <summary>
	/// 測定に必要な情報
	/// </summary>
	FTimerHandle RecalibrationTimer;
	FTimerDynamicDelegate RecalibrationOneTime;
	FTimerManagerTimerParameters RecalibrationParas;
};
