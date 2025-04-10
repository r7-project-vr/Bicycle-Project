﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BikeCharacter.generated.h"

UCLASS()
class BICYCLEPROJECT_API ABikeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABikeCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// <summary>
	/// バイクのメッシュを変更
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ChangeBikeMesh();

private:

	void LoadBikeMesh();

	void LoadMeshComplete();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UStaticMesh> _bikeMesh;

	UStaticMeshComponent* _bike;

};
