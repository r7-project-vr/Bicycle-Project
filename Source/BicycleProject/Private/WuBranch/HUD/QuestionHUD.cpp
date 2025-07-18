// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/HUD/QuestionHUD.h"
#include "Blueprint/UserWidget.h"

AQuestionHUD::AQuestionHUD()
{
	
}

void AQuestionHUD::BeginPlay()
{
	Super::BeginPlay();

	AddWidgetToView();
}

void AQuestionHUD::AddWidgetToView()
{
	if (HUDUIClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDUIClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			HUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HUDWidget is null!"));
		}
	}
}
