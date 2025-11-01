// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WuBranch/Interface/PauseInterface.h"
#include "Celestial.generated.h"

class UDirectionalLightComponent;
class UOrbitalRevolutionComponent;

UCLASS()
class BICYCLEPROJECT_API ACelestial : public AActor, public IPauseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACelestial();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Pause_Implementation() override;
	void ReStart_Implementation() override;
	bool IsPause_Implementation() override;

private:

	/// <summary>
	/// 光源
	/// </summary>
	UPROPERTY(EditAnywhere)
	UDirectionalLightComponent* Light;

	/// <summary>
	/// 公転
	/// </summary>
	UPROPERTY(EditAnywhere)
	UOrbitalRevolutionComponent* Revolution;

	/// <summary>
	/// 停止中か
	/// </summary>
	bool IsPause;
};
