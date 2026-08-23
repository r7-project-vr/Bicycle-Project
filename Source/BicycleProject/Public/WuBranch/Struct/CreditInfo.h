// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/UI/CreditPartWidget.h"
#include "CreditInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct BICYCLEPROJECT_API FCreditInfo : public FTableRowBase
{
	GENERATED_BODY()

	/// <summary>
	/// クレジットに使う一部の構成ウィジェット
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UCreditPartWidget> Sample;

	/// <summary>
	/// タイトル（会社名）
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString Title;

	/// <summary>
	/// 内容
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FCreditContent> Contents;

	/// <summary>
	/// 会社ロゴ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* Logo;

	/// <summary>
	/// キービジュアル
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* Promotional;
};
