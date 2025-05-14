// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/UI/BaseUIActor.h"
#include "QuestionUIActor.generated.h"

class UBikeComponent;
class USplineComponent;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API AQuestionUIActor : public ABaseUIActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AQuestionUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// 問題資料を設置
	/// </summary>
	void SetProblem();

	/// <summary>
	/// 左出口から出る
	/// </summary>
	void UseLeftExit();

	/// <summary>
	/// 右出口から出る
	/// </summary>
	void UseRightExit();

private:

	/// <summary>
	/// プレイヤーが停車エリアに入ったら
	/// </summary>
	/// <param name="bike"></param>
	void HandlePlayerEnterArea(UBikeComponent* bike);

	/// <summary>
	/// 停車エリアに入ったら
	/// </summary>
	UFUNCTION()
	void OnOverlapBeginParkingArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	/// <summary>
	/// オートプレイ対象を一旦記録して
	/// </summary>
	/// <param name="target"></param>
	void SetTarget(UBikeComponent* target);

	/// <summary>
	/// オートプレイ対象を出口へ誘導
	/// </summary>
	void LeadToExit(float DeltaTime);

	/// <summary>
	/// 一時停車領域
	/// </summary>
	class UBoxComponent* _temporaryParkingArea;

	/// <summary>
	/// オートプレイ対象
	/// </summary>
	UBikeComponent* _autoPlayTarget;

	/// <summary>
	/// オートプレイのスタート地点
	/// </summary>
	UPROPERTY(EditAnywhere)
	USplineComponent* _autoPlayStart;

	/// <summary>
	/// 目標の出口
	/// </summary>
	USplineComponent* _exitTarget;

	/// <summary>
	/// オートプレイの左出口
	/// </summary>
	UPROPERTY(EditAnywhere)
	USplineComponent* _exitLeft;

	/// <summary>
	/// オートプレイの右出口
	/// </summary>
	UPROPERTY(EditAnywhere)
	USplineComponent* _exitRight;

	/// <summary>
	/// オートプレイする時の移動速度
	/// </summary>
	UPROPERTY(EditDefaultsOnly)
	float _autoPlayMoveSpeed;

	/// <summary>
	/// オートプレイする時の既に移動した距離
	/// </summary>
	float _movedDistance;
};
