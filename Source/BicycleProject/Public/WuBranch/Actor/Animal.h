
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animal.generated.h"

UCLASS()
class BICYCLEPROJECT_API AAnimal : public ACharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAnimal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// 今のスピードをゲット
	/// </summary>
	/// <returns>スピード</returns>
	float GetCurrentSpeed() const;

private:

	/// <summary>
	/// 追う
	/// </summary>
	void Chase(float DeltaTime);

	/// <summary>
	/// 追う目標
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACharacter> Target;

	/// <summary>
	/// 追い始まる距離
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float StartChaseDistance;

	/// <summary>
	/// スピード
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Speed;
};
