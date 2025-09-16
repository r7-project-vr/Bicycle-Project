// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Actor/Animal.h"
#include "CelebrateAnimal.generated.h"

class UAudioComponent;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API ACelebrateAnimal : public AAnimal
{
	GENERATED_BODY()
	
public:

	ACelebrateAnimal();

protected:
	virtual void BeginPlay() override;

public:

	/// <summary>
	/// 祝う
	/// </summary>
	void Celebrate();

protected:

	virtual void DecideBehavior() override;

	virtual void Action(float DeltaTime) override;

private:

	/// <summary>
	/// 祝うか
	/// </summary>
	bool bIsCelebrating = false;

	/// <summary>
	/// 祝うSE
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* CelebrateChirp;
};
