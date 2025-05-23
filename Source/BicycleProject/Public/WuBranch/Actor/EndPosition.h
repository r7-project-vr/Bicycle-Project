﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndPosition.generated.h"

UCLASS()
class BICYCLEPROJECT_API AEndPosition : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEndPosition();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
	void OnOverlapBeginFinishLine(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	/// <summary>
	/// 今の世界を再読み込み
	/// </summary>
	void RestartLevel();

	/// <summary>
	/// ゴールのメッシュ
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* _mesh;

	/// <summary>
	/// ゴールのコリジョン
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _finishLineCollision;

	UPROPERTY(EditAnywhere, Category = "Effect")
	class UNiagaraSystem* _fireworkEffectLeft;

	UPROPERTY(EditAnywhere, Category = "Effect")
	class UNiagaraSystem* _fireworkEffectRight;

	bool _isReadyToChangeLevel;
};
