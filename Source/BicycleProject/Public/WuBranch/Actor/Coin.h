// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Coin.generated.h"

class UBoxComponent;
class USoundBase;
class UCoinSpawnerComponent;

UCLASS()
class BICYCLEPROJECT_API ACoin : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoin();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="Spawner">生成者</param>
	void Init(UCoinSpawnerComponent* Spawner);

private:

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	/// <summary>
	/// アニメーション
	/// </summary>
	/// <param name="DeltaTime"></param>
	void PlayAnimation(float DeltaTime);

	/// <summary>
	/// 移動のアニメーションをプレイ
	/// </summary>
	/// <param name="DeltaTime"></param>
	void PlayMoveAnimation(float DeltaTime);

	/// <summary>
	/// 回転のアニメーションをプレイ
	/// </summary>
	/// <param name="DeltaTime"></param>
	void PlayRotateAnimation(float DeltaTime);

	/// <summary>
	/// コインを追加
	/// </summary>
	/// <param name="Actor">対象</param>
	void AddCoin(AActor* Actor);

	/// <summary>
	/// メッシュ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	/// <summary>
	/// コリジョン
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Collision;

	/// <summary>
	/// 生成者
	/// </summary>
	UCoinSpawnerComponent* Maker;

	/// <summary>
	/// 回転するか
	/// </summary>
	bool IsRotating;

	/// <summary>
	/// 一回転に必要の秒数
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RotationTime;

	/// <summary>
	/// 回転のディレイ(最小、最大)
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector2D RotationDelay;

	/// <summary>
	/// 回転秒数の計算
	/// </summary>
	float RotateTimeCnt;

	/// <summary>
	/// 基本回転
	/// </summary>
	FRotator BaseRotation;

	/// <summary>
	/// 基本座標
	/// </summary>
	FVector BaseLocation;

	/// <summary>
	/// 角度の計算
	/// </summary>
	float MoveCnt;

	/// <summary>
	/// アニメーション用の移動量
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MoveDistanceForAnimation;

	/// <summary>
	/// 拾った時のSE
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USoundBase* PickupSound;
};
