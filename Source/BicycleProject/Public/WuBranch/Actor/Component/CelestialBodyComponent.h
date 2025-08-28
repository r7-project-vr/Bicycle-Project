// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WuBranch/Struct/MyTime.h"
#include "CelestialBodyComponent.generated.h"

class UDirectionalLightComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UCelestialBodyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCelestialBodyComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// 太陽の角度を取得
	/// </summary>
	/// <returns>太陽の角度</returns>
	float GetSunAngle() const;

	/// <summary>
	/// 今の時刻を取得
	/// </summary>
	/// <returns>時刻</returns>
	FMyTime GetTime();

	/// <summary>
	/// 現実時間とゲーム内時間の比率をゲット　1は同じ、１より大きのはゲーム時間が速い
	/// </summary>
	/// <returns>比率</returns>
	float GetTimeRatio();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDisplayTimeDelegate, FMyTime, nowTime);

	/// <summary>
	/// 時間を表示してくれるUI達
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FDisplayTimeDelegate OnUpdateTime;

private:

	/// <summary>
	/// 太陽か月
	/// </summary>
	UDirectionalLightComponent* Target;

	/// <summary>
	/// 新しい太陽の角度を計算する
	/// </summary>
	/// <param name="sec">ゲーム開始以後どれくらい経った</param>
	/// <returns>太陽の角度</returns>
	float CaculateSunAngle(float Sec) const;

	/// <summary>
	/// 太陽の角度を更新
	/// </summary>
	/// <param name="time"></param>
	void ChangeSunAngle(float Angle);

	/// <summary>
	/// 太陽の角度から時間を計算する
	/// </summary>
	/// <param name="angle">角度</param>
	/// <returns>時間</returns>
	FMyTime ConvertAngleToTime(float Angle);

	/// <summary>
	/// 最初の角度（RollとYawを記録するため）
	/// </summary>
	FRotator OriginAngle;

	/// <summary>
	/// ゲーム内で経った時間
	/// </summary>
	float Time = 0;

	/// <summary>
	/// 太陽の角度
	/// </summary>
	float SunAngle;

	/// <summary>
	/// ゲーム内の時間と現実時間の比率、１より大きい場合はゲーム内の時間が速い、０以下になってはいけない
	/// </summary>
	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"));
	float TIME_RATIO = 5.0f;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"));
	float START_ANGLE = 210.0f;
};
