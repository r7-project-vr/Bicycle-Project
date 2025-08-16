// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RandomFoliageSpawner.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API URandomFoliageSpawner : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URandomFoliageSpawner();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings", meta = (AllowPrivateAccess = "true"))
	UFoliageType* FoliageType;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings", meta = (AllowPrivateAccess = "true"))
	TArray<FBox> SpawnZones;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings", meta = (AllowPrivateAccess = "true"))
	int32 NumberOfInstancesPerZone = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings", Meta = (AllowPrivateAccess = "true"))
	FVector2D RandomYawRotation = FVector2D(0.0f, 360.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings", Meta = (AllowPrivateAccess = "true"))
	FVector2D RandomScale = FVector2D(0.8f, 1.2f);*/
};
