// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndPosition.generated.h"

class UNiagaraComponent;

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
	void OnOverlapBeginFinishLine(AActor* OverlappedActor, AActor* OtherActor);

	/// <summary>
	/// 花火の初期化
	/// </summary>
	void CreateFirework();

	/// <summary>
	/// 花火発射
	/// </summary>
	void Fire();

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

	/// <summary>
	/// ゲームモード
	/// </summary>
	class AQuestionGameMode* _gameMode;

	/// <summary>
	/// 花火
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraComponent* _fireworkEffect1;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraComponent* _fireworkEffect2;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraComponent* _fireworkEffect3;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraComponent* _fireworkEffect4;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraComponent* _fireworkEffect5;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraComponent* _fireworkEffect6;

	/// <summary>
	/// SE
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USoundBase* FireworkSE;

};
