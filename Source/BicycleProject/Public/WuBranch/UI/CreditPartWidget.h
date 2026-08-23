// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WuBranch/Struct/CreditContent.h"
#include "CreditPartWidget.generated.h"

class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BICYCLEPROJECT_API UCreditPartWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/// <summary>
	/// タイトルを設定
	/// </summary>
	/// <param name="Content">タイトル文字</param>
	UFUNCTION(BlueprintCallable)
	void SetTitle(FString Content);

	/// <summary>
	/// 詳細内容を設定
	/// </summary>
	/// <param name="Staffs">内容</param>
	UFUNCTION(BlueprintImplementableEvent)
	void SetDetail(const TArray<FCreditContent>& Staffs);

	/// <summary>
	/// ロゴを設定
	/// </summary>
	/// <param name="Texture">画像</param>
	/// <param name="bMatchSize"></param>
	UFUNCTION(BlueprintCallable)
	void SetLogo(UTexture2D* Texture, bool bMatchSize = false);

	/// <summary>
	/// キービジュアルを設定
	/// </summary>
	/// <param name="Texture">画像</param>
	/// <param name="bMatchSize"></param>
	UFUNCTION(BlueprintCallable)
	void SetPromotional(UTexture2D* Texture, bool bMatchSize = false);

protected:

	/// <summary>
	/// タイトル
	/// </summary>
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleText;

	/// <summary>
	/// 会社ロゴ
	/// </summary>
	UPROPERTY(meta = (BindWidget))
	UImage* LogoImage;

	/// <summary>
	/// キービジュアル
	/// </summary>
	UPROPERTY(meta = (BindWidget))
	UImage* PromotionalImage;
	
};
