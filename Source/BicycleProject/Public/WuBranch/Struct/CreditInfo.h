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
	/// サブタイトル（部署）
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString SubTitle;

	/// <summary>
	/// 内容（スタッフ）
	/// 改行したい場合は新しいStringを追加
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FString> Contents;

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
