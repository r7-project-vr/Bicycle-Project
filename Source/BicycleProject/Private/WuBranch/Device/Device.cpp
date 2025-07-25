// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/Device.h"

UDevice::UDevice()
{
}

bool UDevice::Connect()
{
	return false;
}

bool UDevice::DisConnect()
{
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
