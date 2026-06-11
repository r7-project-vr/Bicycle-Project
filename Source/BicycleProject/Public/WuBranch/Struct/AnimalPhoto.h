// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "AnimalPhoto.generated.h"

/**
 * 撮影した動物の写真
 */
USTRUCT(BlueprintType)
struct BICYCLEPROJECT_API FAnimalPhoto
{
	GENERATED_BODY()

	/// <summary>
	/// 写真
	/// </summary>
	UPROPERTY()
	UTexture2D* Photo;

	/// <summary>
	/// 一枚の写真に写っている全動物の名前
	/// </summary>
	UPROPERTY()
	TArray<FName> Names;

	/// <summary>
	/// レアか
	/// </summary>
	bool bIsRare;
};
