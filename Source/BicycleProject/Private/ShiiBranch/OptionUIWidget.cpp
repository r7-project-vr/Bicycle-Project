// Fill out your copyright notice in the Description page of Project Settings.


#include "ShiiBranch/OptionUIWidget.h"
#include <UntakuBranch/Question.h>

void UOptionUIWidget::SetQuestionAndAnswer(const FQuestion& Question)
{
	questionID = Question.ID;
	SetQuestion(Question.Content);
	SetAnswer(Question.AnswerNum, Question.AnswerContents);
}

void UOptionUIWidget::OnSelectedOption(int32 OptionIndex)
{
	if (OnOptionSelected.IsBound())
	{
		OnOptionSelected.Broadcast(OptionIndex);
	}
}