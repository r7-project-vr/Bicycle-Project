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

	_widget = CreateDefaultSubobject<UWidgetComponent>(FName("widget"));
	_widget->SetupAttachment(RootComponent);
	AddInstanceComponent(_widget);

}

// Called when the game starts or when spawned
void ABaseUIActor::BeginPlay()
{
	Super::BeginPlay();
	

	UQuestionWidget* question = Cast<UQuestionWidget>(_widget->GetWidget());
	if (!question) {
		UE_LOG(LogTemp, Error, TEXT("Widget instance is null!"));
	}
	else {
		question->SetQuestion("text1");
		question->SetAnswer1("text2");
		question->SetAnswer2("text3");
	}
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

