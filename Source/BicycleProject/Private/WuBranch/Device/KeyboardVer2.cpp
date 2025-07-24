// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/KeyboardVer2.h"

UKeyboardVer2::UKeyboardVer2()
{
}

bool UKeyboardVer2::Connect()
{
#if PLATFORM_WINDOWS
	State = EDeviceConnectType::Connected;
	return true;
#else
	return false;
#endif
}

bool UKeyboardVer2::DisConnect()
{
#if PLATFORM_WINDOWS
	State = EDeviceConnectType::UnConnected;
	return true;
#else
	return false;
#endif
}

void UKeyboardVer2::BindMoveEvent_Implementation(UObject* object, FName functionName)
{
}

void UKeyboardVer2::EnableMove_Implementation()
{
}

void UKeyboardVer2::DisableMove_Implementation()
{
}
