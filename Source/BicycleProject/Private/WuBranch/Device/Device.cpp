// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/Device.h"

UDevice::UDevice()
{
}

bool UDevice::Connect()
{
	UE_LOG(LogTemplateDevice, Error, TEXT("The Connect function has not been implemented yet!"));
	return false;
}

bool UDevice::Disconnect()
{
	UE_LOG(LogTemplateDevice, Error, TEXT("The Disconnect function has not been implemented yet.!"));
	return false;
}

FString UDevice::GetName() const
{
	return Name;
}

FString UDevice::GetUUID() const
{
	return UUID;
}

EDeviceConnectType UDevice::GetConnectState()
{
	return State;
}

void UDevice::BindMoveEvent_Implementation(UObject* Object, FName FunctionName)
{
	if (Object)
	{
		FScriptDelegate Delegate;
		Delegate.BindUFunction(Object, FunctionName);
		OnMoveEvent.Add(Delegate);
	}
}

void UDevice::BindSelectLeftEvent_Implementation(UObject* Object, FName FunctionName)
{
	if (Object)
	{
		FScriptDelegate Delegate;
		Delegate.BindUFunction(Object, FunctionName);
		OnSelectLeftEvent.Add(Delegate);
	}
}

void UDevice::BindSelectRightEvent_Implementation(UObject* Object, FName FunctionName)
{
	if (Object)
	{
		FScriptDelegate Delegate;
		Delegate.BindUFunction(Object, FunctionName);
		OnSelectRightEvent.Add(Delegate);
	}
}
