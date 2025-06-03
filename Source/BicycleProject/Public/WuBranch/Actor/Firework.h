// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Firework.generated.h"

UCLASS()
class BICYCLEPROJECT_API AFirework : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFirework();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// 花火を発射する
	/// </summary>
	void Fire();

private:

	UPROPERTY(EditAnywhere, Category = "Effect")
	class UNiagaraComponent* _fireworkEffect;
};
