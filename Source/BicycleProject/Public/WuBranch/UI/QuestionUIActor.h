// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/UI/BaseUIActor.h"
#include "QuestionUIActor.generated.h"

class UBikeComponent;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API AQuestionUIActor : public ABaseUIActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AQuestionUIActor();

private:

	/// <summary>
	/// プレイヤーが停車エリアに入ったら
	/// </summary>
	/// <param name="bike"></param>
	void HandlePlayerEnterArea(UBikeComponent* bike);

	/// <summary>
	/// 停車領域に入ったら
	/// </summary>
	UFUNCTION()
	void OnOverlapBeginParkingArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	/// <summary>
	/// 一時停車領域
	/// </summary>
	class UBoxComponent* _temporaryParkingArea;
};
