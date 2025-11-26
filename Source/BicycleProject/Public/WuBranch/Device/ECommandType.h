// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <ASerialCore/ASerialPacket.h>

/**
 * 
 */
enum class ECommandType : uint8_t
{
	RPMUpdateCheck = 0x20,		// RPM更新フラグ
	RPM = 0x21,					// RPM
	RPS = 0x22,					// RPS
	Revolutions = 0x24,			// 回転数
	RevolutionsReset = 0x25,	// 回転数をリセット
};