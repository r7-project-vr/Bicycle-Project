// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include <Components/WidgetComponent.h>
#include <tokuamaru/TimerWidget.h>
#include "GameFramework/Actor.h"
#include "TimeManager.generated.h"

UCLASS()
class BICYCLEPROJECT_API ATimeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimeManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float timer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* _widgetT;
	UTimerWidget* TimerW;


};
