// Fill out your copyright notice in the Description page of Project Settings.


#include "ShiiBranch/OptionUIWidget.h"
#include <UntakuBranch/Question.h>

void UOptionUIWidget::SetQuestionAndAnswer(const FQuestion& Question)
{
	SetQuestion(Question.Content);
	SetAnswer(Question.AnswerNum, Question.AnswerContents);
}
