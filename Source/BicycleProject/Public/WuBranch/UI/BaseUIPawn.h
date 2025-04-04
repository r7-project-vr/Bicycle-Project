// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseUIPawn.generated.h"

UCLASS()
class BICYCLEPROJECT_API ABaseUIPawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseUIPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	/// <summary>
	/// 停車領域に入ったら
	/// </summary>
	UFUNCTION()
	void OnOverlapBeginParkingArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	/// <summary>
	/// UIを表示させるもの
	/// </summary>
	class UWidgetComponent* _widget;

	/// <summary>
	/// 一時停車領域
	/// </summary>
	class UBoxComponent* _temporaryParkingArea;
};
