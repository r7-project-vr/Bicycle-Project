// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/CreditWidget.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "WuBranch/Struct/CreditInfo.h"
#include "WuBranch/UI/CreditPartWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/CanvasPanelSlot.h"

void UCreditWidget::SetRollType()
{
	PrepareRolling();
}

void UCreditWidget::StartAnimation()
{
	if (!bIsInitCompleted)
	{
		UE_LOG(LogTemp, Error, TEXT("Initialize is not complete!"));
		return;
	}

	if (bIsPlayingAnimation)
	{
		UE_LOG(LogTemp, Error, TEXT("Animation is playing!"));
		return;
	}

	bIsPlayingAnimation = true;

	if (Container)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Container height: %f"), Container->GetDesiredSize().Y));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Container count: %d"), Container->GetChildrenCount()));
	}
		
}

void UCreditWidget::SkipAnimation()
{
	if (!bIsPlayingAnimation)
	{
		UE_LOG(LogTemp, Error, TEXT("Animation is not playing"));
		return;
	}

	bIsPlayingAnimation = false;
}

void UCreditWidget::SetVisibleHeight(float Height)
{
	if (!Container)
	{
		UE_LOG(LogTemp, Error, TEXT("Didnot set container!"));
		return;
	}

	Container->SetRenderTranslation(FVector2D(0.f, Height));
	CurrentOffset = Height;
}

void UCreditWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	bIsInitCompleted = false;
	bIsPlayingAnimation = false;
	ContainerHeight = 0.0f;
	CurrentOffset = 0.0f;

	LoadCreditTable();
}

void UCreditWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsPlayingAnimation)
	{
		UpdateRolling(InDeltaTime);
	}
}

void UCreditWidget::LoadCreditTable()
{
	FStreamableManager& Streamable = UAssetManager::Get().GetStreamableManager();
	Streamable.RequestAsyncLoad(CreditTable.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &UCreditWidget::OnLoadCompleted));
}

void UCreditWidget::OnLoadCompleted()
{
	UDataTable* Table = CreditTable.Get();

	if (Table)
	{
		Table->GetAllRows<FCreditInfo>(TEXT(""), CreditDatas);
		// 前回の子ウィジェットを削除してから新しい子ウィジェットを作成
		Container->ClearChildren();
		PartWidgets.Empty();
		
		CreatePart();

		Container->ForceLayoutPrepass();
		ContainerHeight = Container->GetDesiredSize().Y;
		bIsInitCompleted = true;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Load Credit Table Complete"));
		StartAnimation();
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Load Credit Table failed!"));
}

void UCreditWidget::CreatePart()
{
	if (!Container)
	{
		UE_LOG(LogTemp, Error, TEXT("Didnot set container!"));
		return;
	}

	int CreditCount = CreditDatas.Num();
	for (int Index = 0; Index < CreditCount; Index++)
	{
		FCreditInfo* Info = CreditDatas[Index];
		if(!Info || !Info->Sample)
		{
			UE_LOG(LogTemp, Error, TEXT("Credit info or sample is null at index %d!"), Index);
			continue;
		}

		UUserWidget* CreditPart = CreateWidget(GetWorld(), Info->Sample);
		if (UCreditPartWidget* PartWidget = Cast<UCreditPartWidget>(CreditPart))
		{
			InitializePartWidget(PartWidget, Info);
			PartWidgets.Add(PartWidget);
			UVerticalBoxSlot* BoxSlot = Container->AddChildToVerticalBox(PartWidget);
			// 最初のパート以外はパディングを設定
			if (Index != 0)
				BoxSlot->SetPadding(PartPadding);
			BoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		}
	}
}

void UCreditWidget::InitializePartWidget(UCreditPartWidget* Target, FCreditInfo* Data)
{
	if (!Target || !Data)
	{
		UE_LOG(LogTemp, Error, TEXT("Part widget or data is null!"));
		return;
	}
	
	Target->SetTitle(Data->Title);
	Target->SetSubTitle(Data->SubTitle);
	Target->SetLogo(Data->Logo);
	Target->SetPromotional(Data->Promotional);
	Target->SetDetail(Data->Contents);
}

void UCreditWidget::PrepareRolling()
{
	if (!Container)
	{
		UE_LOG(LogTemp, Error, TEXT("Container is null!"));
		return;
	}

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Container->Slot))
	{
		CanvasSlot->SetAnchors(FAnchors(0.5f, 0.f, 0.5f, 0.f));
		CanvasSlot->SetPosition(FVector2D::ZeroVector);
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.f));
		CanvasSlot->SetAutoSize(true);
	}
}

void UCreditWidget::UpdateRolling(float DeltaTime)
{
	CurrentOffset -= RollSpeed * DeltaTime;
	Container->SetRenderTranslation(FVector2D(0.0f, CurrentOffset));

	if (CurrentOffset < -ContainerHeight)
	{
		bIsPlayingAnimation = false;
	}
}
