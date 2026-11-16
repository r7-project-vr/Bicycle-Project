// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/TutorialMenuWidget.h"

void UTutorialMenuWidget::NotifyHaveChoosed()
{
	if (TutorialChooseCompletedDelegate.IsBound())
	{
		TutorialChooseCompletedDelegate.Broadcast();
	}
}