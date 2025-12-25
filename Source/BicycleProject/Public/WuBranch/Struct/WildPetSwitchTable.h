// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WildPetSwitchTable.generated.h"

/**
 * 野生動物からペットへの変換表
 */
USTRUCT(BlueprintType)
struct BICYCLEPROJECT_API FWildPetSwitchTable : public FTableRowBase
{
	GENERATED_BODY()

	/// <summary>
	/// 野生動物のID
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 WildAnimalID;

	/// <summary>
	/// ペットのID
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 PetAnimalID;
};
