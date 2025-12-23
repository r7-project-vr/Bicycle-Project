// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BikeMovementComponent.generated.h"

class UBikeComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UBikeMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBikeMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// スピードを0まで下げる
	/// </summary>
	void ReduceVelocityTo0();

	/// <summary>
	/// オートプレイを起動
	/// </summary>
	void EnableAutoPlay();

	/// <summary>
	/// オートプレイをキャンセル
	/// </summary>
	void DisableAutoPlay();

	/// <summary>
	/// 今オートプレイしているですか
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	UFUNCTION(BlueprintCallable)
	bool GetIsAutoPlay() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateAutoPlayDelegate, bool, bIsAutoPlay);
	/// <summary>
	/// オートプレイ
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FUpdateAutoPlayDelegate OnUpdateAutoPlayEvent;

	/// <summary>
	/// オートプレイの際に指定された位置についたの通知
	/// </summary>
	/// <param name=""></param>
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FArriveLocationDelegate, UBikeMovementComponent*, Movement);
	FArriveLocationDelegate OnArrivedLocationEvent;

	/// <summary>
	/// オートプレイする時同期したい座標
	/// </summary>
	/// <param name="pos">座標</param>
	void SetSynchPos(FVector pos);

private:

	/// <summary>
	/// 慣性の処理
	/// </summary>
	void HandleInertia(float DeltaTime);

	/// <summary>
	/// 移動
	/// </summary>
	/// <param name="Direction">移動方向</param>
	UFUNCTION()
	virtual void OnMove(FVector2D Direction);

	/// <summary>
	/// オートプレイの更新を通知
	/// </summary>
	void NotifyAutoPlay();

	/// <summary>
	/// 所有者
	/// </summary>
	ACharacter* Owner;

	/// <summary>
	/// バイクコンポネント
	/// </summary>
	UBikeComponent* BikeCom;

	/// <summary>
	/// 慣性のスピード
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	float InertiaSpeed;

	/// <summary>
	/// 慣性の速度(実際に作用する速度)
	/// </summary>
	FVector InertiaVelocity;

	/// <summary>
	/// 慣性の減衰
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	float InertiaDamping;

	/// <summary>
	/// オートプレイ
	/// </summary>
	bool bIsAutoPlay;

	/// <summary>
	/// オートプレイする時同期したい位置
	/// </summary>
	FVector SynchronizePos;

	/// <summary>
	/// 移動の入力あるか
	/// </summary>
	bool bHasMovInput;
};
