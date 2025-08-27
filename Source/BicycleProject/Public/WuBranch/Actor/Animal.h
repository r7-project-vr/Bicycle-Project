
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animal.generated.h"

UCLASS()
class BICYCLEPROJECT_API AAnimal : public AActor
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

private:

	/// <summary>
	/// 追う
	/// </summary>
	void Chase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	/// <summary>
	/// 追う目標
	/// </summary>
	TObjectPtr<ACharacter> Target;

	/// <summary>
	/// 追い始まる距離
	/// </summary>
	float StartChaseDistance;

	/// <summary>
	/// 毎フレーム追う距離(%)
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ChaseDistancePerFrame;
};
