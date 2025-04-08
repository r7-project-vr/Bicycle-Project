// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BikeComponent.generated.h"

class UDeviceManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UBikeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBikeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// 強制的にコントロールをオンにする
	/// </summary>
	void OpenForcedControl();

	/// <summary>
	/// 強制的にコントロールをオフにする
	/// </summary>
	void CloseForcedControl();

	/// <summary>
	/// スピードを0まで下げる
	/// </summary>
	void ReduceVelocityTo0();

private:

	/// <summary>
	/// 慣性の処理
	/// </summary>
	void HandleInertia(float DeltaTime);

	/// <summary>
	/// 移動
	/// </summary>
	/// <param name="direction">移動方向</param>
	UFUNCTION()
	void OnMove(FVector2D direction);

	/// <summary>
	/// 左の答えを選ぶ
	/// </summary>
	UFUNCTION()
	void OnSelectLeftAnswer();

	/// <summary>
	/// 右の答えを選ぶ
	/// </summary>
	UFUNCTION()
	void OnSelectRightAnswer();

	/// <summary>
	/// 答えを選んだの処理
	/// </summary>
	/// <param name="dir">曲がりたい方向</param>
	void HandleSelectAnswer(FRotator dir);

	/// <summary>
	/// 答えを選ぶ動作を機能させない
	/// </summary>
	void DisableSelectAnswer();

	/// <summary>
	/// デバイスマネージャー
	/// </summary>
	UDeviceManager* _deviceManager;

	/// <summary>
	/// スピード
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	float _speed;

	/// <summary>
	/// 単位速度
	/// </summary>
	const float _unitSpeed = 1;

	/// <summary>
	/// 強制的にコントロールのスウィッチ
	/// </summary>
	bool _isForcedControl;

	/// <summary>
	/// 慣性の速度
	/// </summary>
	FVector _inertiaVelocity;

	/// <summary>
	/// 慣性の減衰
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	float _inertiaDamping;

	class UCapsuleComponent* _player;
};
