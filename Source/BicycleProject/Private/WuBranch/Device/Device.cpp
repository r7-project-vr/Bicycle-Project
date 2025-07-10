// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/Device.h"

UDevice::UDevice()
{
}

FString UDevice::GetName_Implementation() const
{
	return _name;
}

FString UDevice::GetUUID_Implementation() const
{
	return _uuid;
}

EDeviceConnectType UDevice::GetConnectState_Implementation() const
{
	return _state;
}

void UDevice::BindMoveEvent_Implementation(UObject* object, FName functionName)
{
	if (object)
	{
		FScriptDelegate delegate;
		delegate.BindUFunction(object, functionName);
		_onMoveEvent.Add(delegate);
	}
}

void UDevice::BindSelectLeftEvent_Implementation(UObject* object, FName functionName)
{
	if (object)
	{
		FScriptDelegate delegate;
		delegate.BindUFunction(object, functionName);
		_onSelectLeftEvent.Add(delegate);
	}
}

void UDevice::BindSelectRightEvent_Implementation(UObject* object, FName functionName)
{
	if (object)
	{
		FScriptDelegate delegate;
		delegate.BindUFunction(object, functionName);
		_onSelectRightEvent.Add(delegate);
	}
}
