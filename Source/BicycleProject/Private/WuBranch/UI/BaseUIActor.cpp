// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/BaseUIActor.h"
#include <Components/WidgetComponent.h>
#include <tokuamaru/QuestionWidget.h>

// Sets default values
ABaseUIActor::ABaseUIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	AddInstanceComponent(RootComponent);

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

void ABaseUIActor::DisplayUI()
{
	_widget->GetWidget()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void ABaseUIActor::NotDisplayUI()
{
	_widget->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
}

UWidgetComponent* ABaseUIActor::GetWidgetComponent()
{
	return _widget;
}

