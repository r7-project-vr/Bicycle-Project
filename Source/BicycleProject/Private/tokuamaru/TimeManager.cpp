// Fill out your copyright notice in the Description page of Project Settings.


#include "tokuamaru/TimeManager.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATimeManager::ATimeManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	AddInstanceComponent(RootComponent);

	_widgetT = CreateDefaultSubobject<UWidgetComponent>(FName("widget"));
	_widgetT->SetupAttachment(RootComponent);
	AddInstanceComponent(_widgetT);

}

// Called when the game starts or when spawned
void ATimeManager::BeginPlay()
{
	Super::BeginPlay();
	timer = 0.0f;
	TimerW = Cast<UTimerWidget>(_widgetT->GetWidget());
	
}

// Called every frame
void ATimeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timer += DeltaTime;
	if (!TimerW) {
		UE_LOG(LogTemp, Error, TEXT("TimerW NULLPtr!"));
	}
	else {
		TimerW->SetTimer(timer);
	}

}

