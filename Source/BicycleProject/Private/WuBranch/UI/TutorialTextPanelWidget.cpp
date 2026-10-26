// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/TutorialTextPanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UTutorialTextPanelWidget::SetDialogueText(const FText& Text)
{
	if (!PanelText)
		return;

	PanelText->SetText(Text);

	int Rows = GetRowNums(Text);
	SetBorder(Rows);
}

int UTutorialTextPanelWidget::GetRowNums(const FText& Text)
{
	if (Text.IsEmpty())
		return 0;

	// 改行文字で分割して行数をカウント
	TArray<FString> Lines;
	Text.ToString().ParseIntoArrayLines(Lines);
	return Lines.Num();
}

void UTutorialTextPanelWidget::SetBorder(int Rows)
{
	if (!PanelText || !PanelBorder)
		return;

	FMargin PaddingValue = BaseBorder + BorderSizeOfRow * FMath::Max(0, Rows - 1);
	PanelBorder->SetPadding(FMargin(PaddingValue));
	//PanelText->SetMargin(FMargin(PaddingValue));
}
