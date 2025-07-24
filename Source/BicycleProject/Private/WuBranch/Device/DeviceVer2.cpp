// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/DeviceVer2.h"

UDeviceVer2::UDeviceVer2()
{
	State = EDeviceConnectType::UnConnected;
}

bool UDeviceVer2::Connect()
{
	return false;
}

bool UDeviceVer2::DisConnect()
{
	return false;
}

FString UDeviceVer2::GetName() const
{
	return Name;
}

FString UDeviceVer2::GetUUID() const
{
	return UUID;
}

EDeviceConnectType UDeviceVer2::GetConnectState()
{
	return State;
}
