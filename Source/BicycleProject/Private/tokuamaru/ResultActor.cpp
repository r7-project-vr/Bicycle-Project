// Fill out your copyright notice in the Description page of Project Settings.


#include "tokuamaru/ResultActor.h"
#include "Components/InputComponent.h"
#include "tokuamaru/ResultWidget.h"

// Sets default values
AResultActor::AResultActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* scene = CreateDefaultSubobject<USceneComponent>(FName("Scene"));
	RootComponent = scene;
	AddInstanceComponent(RootComponent);

	_widgetResult = CreateDefaultSubobject<UWidgetComponent>(FName("widget"));
	_widgetResult->SetupAttachment(RootComponent);
	AddInstanceComponent(_widgetResult);

}

// Called when the game starts or when spawned
void AResultActor::BeginPlay()
{
	Super::BeginPlay();

	answerTrue = 1.0f;
	answerFalse = 0.0f;
	resultWidget = Cast<UResultWidget>(_widgetResult->GetWidget());
	resultWidget->SetRotation(answerTrue);
	resultWidget->SetRotationF(answerFalse);
}

// Called every frame
void AResultActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (answerTrue > 0.1f) {
		answerTrue -= 0.01;
	}

	if (answerFalse < 0.9f) {
		answerFalse += 0.01f;
	}
	resultWidget->SetRotation(answerTrue);
	resultWidget->SetRotationF(answerFalse);
}

