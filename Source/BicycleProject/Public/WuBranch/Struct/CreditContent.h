// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CreditContent.generated.h"

USTRUCT(BlueprintType)
struct BICYCLEPROJECT_API FCreditContent
{
	GENERATED_BODY()

	/// <summary>
	/// サブタイトル
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString SubTitle;

	/// <summary>
	/// 内容
	/// 改行したい場合は新しいStringを追加
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FString> Contents;
};
