// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShopItem.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct BICYCLEPROJECT_API FShopItem : public FTableRowBase
{
	GENERATED_BODY()

	/// <summary>
	/// ID
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ID;

	/// <summary>
	/// ショップの中のアイコン
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon;

	/// <summary>
	/// 値段
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 Price;

	/// <summary>
	/// 解放する写真枚数
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 UnLockLimit;
};
