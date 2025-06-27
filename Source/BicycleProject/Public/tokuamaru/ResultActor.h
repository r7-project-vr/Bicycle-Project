// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WuBranch/QuestionGameMode.h"
#include "WuBranch/Bike/BikeComponent.h"
#include <tokuamaru/ResultWidget.h>
#include <Components/WidgetComponent.h>
#include "ResultActor.generated.h"

UCLASS()
class BICYCLEPROJECT_API AResultActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResultActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float answerTrue;
	float answerFalse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* _widgetResult;

	UResultWidget* resultWidget;


	AQuestionGameMode* gamemode;

	UBikeComponent* bikeComponent;
	bool autoplay;



};
