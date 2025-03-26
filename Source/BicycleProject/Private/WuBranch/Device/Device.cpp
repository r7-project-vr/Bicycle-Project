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

EDeviceConnectType UDevice::GetConnectState_Implementation()
{
	return _state;
}

void UDevice::BindMoveEvent_Implementation(UObject* Object, FName functionName)
{
	if (Object)
	{
		FScriptDelegate delegate;
		delegate.BindUFunction(Object, functionName);
		_onMoveEvent.Add(delegate);
	}
}
