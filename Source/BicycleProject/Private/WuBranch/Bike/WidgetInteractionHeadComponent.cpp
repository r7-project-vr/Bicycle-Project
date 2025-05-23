// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/WidgetInteractionHeadComponent.h"
#include "Components/WidgetComponent.h"
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplay.h"
#include <WuBranch/UI/QuestionUIActor.h>

UWidgetInteractionHeadComponent::UWidgetInteractionHeadComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

	// 基本設定
	TraceChannel = ECC_GameTraceChannel1;
	InteractionDistance = 2000.0f;

	OnHoveredWidgetChanged.AddDynamic(this, &UWidgetInteractionHeadComponent::OnHoverWidget);
}

void UWidgetInteractionHeadComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWidgetInteractionHeadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWidgetInteractionHeadComponent::EnableHintLine()
{
	bShowDebug = true;
}

void UWidgetInteractionHeadComponent::DisableHintLine()
{
	bShowDebug = false;
}

void UWidgetInteractionHeadComponent::OnHoverWidget(UWidgetComponent* WidgetComponent, UWidgetComponent* PreviousWidgetComponent)
{
	// 前に隠しているウィジェットがあれば、表示させる
	if(WidgetComponent)
	{
		//未回答の問題だけ表示する
		AQuestionUIActor* question = Cast<AQuestionUIActor>(WidgetComponent->GetOwner());
		if (!question->GetAnsweredStatus())
		{
			UUserWidget* Widget = WidgetComponent->GetWidget();
			Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			EnableHintLine();
		}
	}
}

bool UWidgetInteractionHeadComponent::IsVRConnect() const
{
	return GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->XRSystem->GetHMDDevice()->IsHMDConnected();
}
