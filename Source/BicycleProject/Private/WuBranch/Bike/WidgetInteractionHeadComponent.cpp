// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/WidgetInteractionHeadComponent.h"
#include "Components/WidgetComponent.h"
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplay.h"
#include <WuBranch/UI/QuestionUIActor.h>
#include <WuBranch/QuestionGameMode.h>

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
	// ゲーム終了した場合何もしない
	AQuestionGameMode* questionGameMode = Cast<AQuestionGameMode>(GetWorld()->GetAuthGameMode());
	// 2025.6.27 start tokumaru questionGameModeのnull参照対策
	if (questionGameMode) {
	
		if (questionGameMode->IsGameFailed() || questionGameMode->IsGameClear())
			return;
	}
	// 2025.6.27 end 

	// 前に隠しているウィジェットがあれば、表示させる
	if(WidgetComponent)
	{
		//未回答の問題だけ表示する
		AQuestionUIActor* question = Cast<AQuestionUIActor>(WidgetComponent->GetOwner());
		// 2025.6.27 start tokumaru questionGameModeのnull参照対策
		if (question) {
			if (!question->GetAnsweredStatus())
			{
				UUserWidget* Widget = WidgetComponent->GetWidget();
				Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				EnableHintLine();
			}
		}
		// 2025.6.27 end 
	}
}

bool UWidgetInteractionHeadComponent::IsVRConnect() const
{
	return GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->XRSystem->GetHMDDevice()->IsHMDConnected();
}
