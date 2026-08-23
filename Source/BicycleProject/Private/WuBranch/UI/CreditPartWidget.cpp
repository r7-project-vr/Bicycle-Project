// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/CreditPartWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UCreditPartWidget::SetTitle(FString Content)
{
	if (TitleText)
	{
		if (!Content.IsEmpty())
		{
			TitleText->SetText(FText::FromString(Content));
			TitleText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}	
		else
			TitleText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCreditPartWidget::SetLogo(UTexture2D* Texture, bool bMatchSize)
{
	if (LogoImage)
	{
		if (Texture)
		{
			LogoImage->SetBrushFromTexture(Texture, bMatchSize);
			LogoImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
			LogoImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCreditPartWidget::SetPromotional(UTexture2D* Texture, bool bMatchSize)
{
	if (PromotionalImage)
	{
		if (Texture)
		{
			PromotionalImage->SetBrushFromTexture(Texture, bMatchSize);
			PromotionalImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
			PromotionalImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}
