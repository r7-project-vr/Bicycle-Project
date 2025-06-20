// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/CustomDevice.h"

//DEFINE_LOG_CATEGORY(LogTemplateDevice);

UCustomDevice::UCustomDevice()
	: _defaultActionSwitch(false)
	, _selectAnswerSwitch(false)
{
}

void UCustomDevice::Init()
{
	EnableDefaultActions_Implementation();
}

void UCustomDevice::EnableDefaultActions_Implementation()
{
	_defaultActionSwitch = true;
}

void UCustomDevice::DisableDefaultActions_Implementation()
{
	_defaultActionSwitch = false;
}

void UCustomDevice::EnableSelectAnswerActions_Implementation()
{
	_selectAnswerSwitch = true;
}

void UCustomDevice::DisableSelectAnswerActions_Implementation()
{
	_selectAnswerSwitch = false;
}

void UCustomDevice::OnMove()
{
	// (Y, X)
	// Y: 前後, X: 左右(無視)
	FVector2D moveVector(0, 0);

	if (_defaultActionSwitch && _onMoveEvent.IsBound())
		_onMoveEvent.Broadcast(moveVector);
}

void UCustomDevice::OnSelectLeftAnswer()
{
	if (_selectAnswerSwitch && _onSelectLeftEvent.IsBound())
		_onSelectLeftEvent.Broadcast();
}

void UCustomDevice::OnSelectRightAnswer()
{
	if (_selectAnswerSwitch && _onSelectRightEvent.IsBound())
		_onSelectRightEvent.Broadcast();
}
