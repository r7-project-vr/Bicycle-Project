// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/WiredDevice.h"

UWiredDevice::UWiredDevice()
	: MoveSwitch(false)
	, SelectSwitch(false)
{
}

void UWiredDevice::Init()
{
	EnableMoveAction_Implementation();
}

bool UWiredDevice::Connect()
{
	return false;
}

bool UWiredDevice::Disconnect()
{
	return false;
}

void UWiredDevice::EnableMoveAction_Implementation()
{
	MoveSwitch = true;
}

void UWiredDevice::DisableMoveAction_Implementation()
{
	MoveSwitch = false;
}

void UWiredDevice::EnableSelectAnswerAction_Implementation()
{
	SelectSwitch = true;
}

void UWiredDevice::DisableSelectAnswerAction_Implementation()
{
	SelectSwitch = false;
}

void UWiredDevice::FindDevice()
{
}

