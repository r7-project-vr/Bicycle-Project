// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/DeviceInterface.h"

// Add default functionality here for any IDeviceInterface functions that are not pure virtual.

float IDeviceInterface::GetValue()
{
	UE_LOG(LogTemp, Error, TEXT("Did not implement GetValue()!"));
	return 0.0f;
}

bool IDeviceInterface::Connect()
{
	UE_LOG(LogTemp, Error, TEXT("Did not implement Connect()!"));
	return false;
}

bool IDeviceInterface::Disconnect()
{
	UE_LOG(LogTemp, Error, TEXT("Did not implement Disconnect()!"));
	return false;
}
