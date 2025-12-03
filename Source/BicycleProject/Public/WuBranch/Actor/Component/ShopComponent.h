// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShopComponent.generated.h"

struct FShopItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UShopComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShopComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// アイテムを購入
	/// </summary>
	/// <param name="Buyer">購入者</param>
	/// <param name="ItemID">アイテムID</param>
	UFUNCTION(BlueprintCallable)
	void BuyItem(int ItemID);

	/// <summary>
	/// すべてのアイテムをゲット
	/// </summary>
	/// <returns>アイテム</returns>
	TArray<FShopItem*> GetShopItems() const;

	/// <summary>
	/// アイテム更新の通知
	/// </summary>
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateShopItemsDelegate,const TArray<FShopItem>, Items);

	UPROPERTY(BlueprintAssignable)
	FUpdateShopItemsDelegate OnUpdateItems;

private:
	/// <summary>
	/// ショップの商品テーブルをロード
	/// </summary>
	void LoadShopItemTable();

	/// <summary>
	/// 購入失敗
	/// </summary>
	void BuyFailed();

	/// <summary>
	/// アイテム更新を通知する
	/// </summary>
	void NotifyUpdateItems();

	/// <summary>
	/// ショップの商品テーブル
	/// </summary>
	/// 実行する時読み込まれてないことがわかった、一旦ソフト参照を使わない、あとでAsset Managerを使う
	UPROPERTY(EditDefaultsOnly, meta = (RequiredAssetDataTags = "RowStructure=/Script/SimpleShooter.ShopItemTableRow"))
	TSoftObjectPtr<UDataTable> ShopItemsTable;

	/*UPROPERTY(EditDefaultsOnly, meta = (RequiredAssetDataTags = "RowStructure=/Script/SimpleShooter.ShopItemTableRow"))
	UDataTable* ShopItemsTable;*/

	/// <summary>
	/// 商品データ
	/// </summary>
	TArray<FShopItem*> ShopItemsDatas;

	/// <summary>
	/// 購入失敗のサウンド
	/// </summary>
	UPROPERTY(EditDefaultsOnly)
	USoundBase* BuyFailSound;

};
