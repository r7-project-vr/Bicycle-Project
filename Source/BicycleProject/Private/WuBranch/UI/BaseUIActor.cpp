// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/BaseUIActor.h"
#include <Components/WidgetComponent.h>

// Sets default values
ABaseUIActor::ABaseUIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	AddInstanceComponent(RootComponent);

	_widget = CreateDefaultSubobject<UWidgetComponent>(FName("widget"));
	_widget->SetupAttachment(RootComponent);
	AddInstanceComponent(_widget);

}

// Called when the game starts or when spawned
void ABaseUIActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseUIActor::Display()
{
	_widget->Activate(true);
}

void ABaseUIActor::NotDisplay()
{
	_widget->Activate(false);
}

