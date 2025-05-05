// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BikeComponent.generated.h"


class AQuestionUIActor;

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
	/// スピードを0まで下げる
	/// </summary>
	void ReduceVelocityTo0();

	/// 左の答えを選ぶ
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnSelectLeftAnswer();

	/// <summary>
	/// 右の答えを選ぶ
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnSelectRightAnswer();

	/// <summary>
	/// オートプレイを起動
	/// </summary>
	/// <param name="actor"></param>
	void EnableAutoPlay(AQuestionUIActor* actor);

	/// <summary>
	/// オートプレイをキャンセル
	/// </summary>
	void DisableAutoPlay();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="pos"></param>
	void SetSynchPos(FVector pos);

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
	/// VRデバイスに接続しているか
	/// </summary>
	/// <returns>true: はい / false: いいえ</returns>
	bool IsVRConnect() const;

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
	/// スピード
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	float _speed;

	/// <summary>
	/// 単位速度
	/// </summary>
	const float _unitSpeed = 1;

	/// <summary>
	/// 慣性の速度
	/// </summary>
	FVector _inertiaVelocity;

	/// <summary>
	/// 慣性の減衰
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	float _inertiaDamping;

	/// <summary>
	/// オートプレイ
	/// </summary>
	bool _isAutoPlay;

	/// <summary>
	/// オートプレイする時同期したい位置
	/// </summary>
	FVector _synchronizePos;

	AQuestionUIActor* _questionActor;
};
