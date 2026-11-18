// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialIndicator.generated.h"

UCLASS()
class BICYCLEPROJECT_API ATutorialIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialIndicator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// 指す目標を設定
	/// </summary>
	/// <param name="Target">目標</param>
	UFUNCTION(BlueprintCallable)
	void SetTarget(FVector Target);

	/// <summary>
	/// 表示する
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ShowArrow();

	/// <summary>
	/// 隠す
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void HideArrow();

private:

	/// <summary>
	/// 自身の位置を決定する
	/// </summary>
	void DecideSelfLocation();

	/// <summary>
	/// 回転
	/// </summary>
	void FaceToTarget();

	/// <summary>
	/// 移動アニメーション
	/// </summary>
	/// <param name="DeltaTime"></param>
	void MoveAnimation(float DeltaTime);

	/// <summary>
	/// メッシュ
	/// </summary>
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	/// <summary>
	/// プレイヤー
	/// </summary>
	ACharacter* Player;

	/// <summary>
	/// 目標の位置
	/// </summary>
	FVector TargetLocation;

	/// <summary>
	/// 回転の時間
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float RotationSpeed;

	/// <summary>
	/// 目標との距離
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float DistanceFromTarget;

	/// <summary>
	/// 移動アニメーションのカウンター
	/// </summary>
	float MoveAnimationCounter;

	/// <summary>
	/// アニメーション用の移動距離
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float MoveDistanceForAnimation;
};
