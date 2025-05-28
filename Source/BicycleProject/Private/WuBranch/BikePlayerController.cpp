// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/BikePlayerController.h"
#include <Kismet/KismetSystemLibrary.h>

void ABikePlayerController::SetPlayerEnabledState(bool playerEnabled)
{
	if (playerEnabled)
	{
		GetPawn()->EnableInput(this);
	}
	else
	{
		GetPawn()->DisableInput(this);
	}
}
