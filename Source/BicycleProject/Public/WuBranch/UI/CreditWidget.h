// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Layout/Margin.h"
#include "CreditWidget.generated.h"

struct FCreditInfo;
struct FMargin;
class UCreditPartWidget;
class UVerticalBox;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UCreditWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/// <summary>
	/// 
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SetRollType();

	/// <summary>
	/// アニメーションを開始
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void StartAnimation();

	/// <summary>
	/// アニメーションを省略して終了
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SkipAnimation();

	/// <summary>
	/// ウィジェットの表示高さを設定する
	/// </summary>
	/// <param name="Height"></param>
	UFUNCTION(BlueprintCallable)
	void SetVisibleHeight(float Height);

protected:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/// <summary>
	/// クレジットテーブルをロード
	/// </summary>
	void LoadCreditTable();

	/// <summary>
	/// クレジットテーブル
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (RequiredAssetDataTags = "RowStructure=/Script/BicycleProject.CreditInfo"))
	TSoftObjectPtr<UDataTable> CreditTable;

	/// <summary>
	/// クレジットデータ
	/// </summary>
	TArray<FCreditInfo*> CreditDatas;

	/// <summary>
	/// コンテナー
	/// </summary>
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* Container;

	/// <summary>
	/// クレジットパートの間隔
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FMargin PartPadding;

	/// <summary>
	/// 初期化完了のフラグ
	/// </summary>
	UPROPERTY(BlueprintReadOnly)
	bool bIsInitCompleted = false;

	/// <summary>
	/// アニメーション開始のフラグ
	/// </summary>
	UPROPERTY(BlueprintReadOnly)
	bool bIsPlayingAnimation = false;

	/// <summary>
	/// ローリングスピード
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RollSpeed;

private:

	/// <summary>
	/// ロード完了
	/// </summary>
	UFUNCTION()
	void OnLoadCompleted();

	/// <summary>
	/// クレジットパートを作成
	/// </summary>
	void CreatePart();

	/// <summary>
	/// クレジットパートを初期化
	/// </summary>
	void InitializePartWidget(UCreditPartWidget* Target, FCreditInfo* Data);

	/// <summary>
	/// ローリング効果の準備
	/// </summary>
	void PrepareRolling();

	/// <summary>
	/// ローリング効果の更新
	/// </summary>
	/// <param name="DeltaTime"></param>
	void UpdateRolling(float DeltaTime);

	/// <summary>
	/// クレジットパートのウィジェット
	/// </summary>
	TArray<UCreditPartWidget*> PartWidgets;

	/// <summary>
	/// クレジットリストの高さ
	/// </summary>
	float ContainerHeight = 0.0f;

	/// <summary>
	/// 
	/// </summary>
	float CurrentOffset = 0.0f;
};
