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
	/// 移動
	/// </summary>
	/// <param name="direction">移動方向</param>
	UFUNCTION()
	void OnMove(FVector2D direction);

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
	const float _unitSpeed = 10000;

	/// <summary>
	/// 強制的にコントロールのスウィッチ
	/// </summary>
	bool _isForcedControl;

	class UCapsuleComponent* _player;
};
