// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialTextPanelWidget.generated.h"

class UBorder;
class UTextBlock;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UTutorialTextPanelWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/// <summary>
	/// ダイアログのテキストを設定
	/// </summary>
	/// <param name="Text">内容</param>
	UFUNCTION(BlueprintCallable)
	void SetDialogueText(const FText& Text);

	/// <summary>
	/// 説明開始する
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StartExplanation();


	/// <summary>
	/// 説明終了する
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void FinishExplanation();

private:

	/// <summary>
	/// 行数を取得
	/// </summary>
	/// <param name="Text">内容</param>
	/// <returns>行数</returns>
	int GetRowNums(const FText& Text);

	/// <summary>
	/// 行数に応じてボーダーサイズを設定
	/// </summary>
	/// <param name="Rows">行数</param>
	void SetBorder(int Rows);

	/// <summary>
	/// ダイアログのボーダー
	/// </summary>
	UPROPERTY(meta = (BindWidget))
	UBorder* PanelBorder;

	/// <summary>
	/// ダイアログの内容
	/// </summary>
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PanelText;

	/// <summary>
	/// 基本のボーダーサイズ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	FMargin BaseBorder;

	/// <summary>
	/// 一行ごと増えるボーダーサイズ、1行のみなら増えない
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	FMargin BorderSizeOfRow;
};
