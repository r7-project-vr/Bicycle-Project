// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/BaseUIPawn.h"
#include <Components/WidgetComponent.h>

// Sets default values
ABaseUIPawn::ABaseUIPawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	widget = CreateDefaultSubobject<UWidgetComponent>(FName("widget"));
	widget->SetupAttachment(RootComponent);
	AddInstanceComponent(widget);
}

// Called when the game starts or when spawned
void ABaseUIPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseUIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

