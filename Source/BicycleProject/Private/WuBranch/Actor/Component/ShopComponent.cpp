// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/ShopComponent.h"
#include <Kismet/GameplayStatics.h>
#include "WuBranch/Struct/ShopItem.h"
#include <WuBranch/MyGameInstance.h>

// Sets default values for this component's properties
UShopComponent::UShopComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UShopComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	LoadShopItemTable();
	NotifyUpdateItems();
}

// Called every frame
//void UShopComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UShopComponent::BuyItem(int ItemID)
{
	// アイテムを特定
	FShopItem* ShopItem = *(ShopItemsDatas.FindByPredicate([ItemID](const FShopItem* Item) {
		return Item->ID == ItemID;
		}));

	if (UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		// 金をチェック
		int CurrentCoins = GameInstance->GetTotalCoins();
		if (CurrentCoins < ShopItem->Price)
		{
			// 購入失敗　金足りない
			BuyFailed();
			return;
		}

		// 動物の数が最大に持っているか
		if (GameInstance->HasMaxAnimals())
		{
			BuyFailed();
			return;
		}

		// 購入
		GameInstance->SetTotalCoins(CurrentCoins - ShopItem->Price);
		GameInstance->AddAnimal(ShopItem->ID);
	}
}

TArray<FShopItem*> UShopComponent::GetShopItems() const
{
	return ShopItemsDatas;
}

void UShopComponent::LoadShopItemTable()
{
	if (ShopItemsTable && ShopItemsDatas.Num() == 0)
	{
		UDataTable* Table = ShopItemsTable.Get();
		if (Table)
			Table->GetAllRows<FShopItem>(TEXT(""), ShopItemsDatas);
		else
			UE_LOG(LogTemp, Error, TEXT("Load Shop item Table failed!"));

		//ShopItemsTable->GetAllRows<FShopItemTableRow>(TEXT(""), ShopItemsDatas);
	}
}

void UShopComponent::BuyFailed()
{
	if (BuyFailSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), BuyFailSound);
	}
}

void UShopComponent::NotifyUpdateItems()
{
	if (OnUpdateItems.IsBound())
	{
		TArray<FShopItem> Items;
		for (const FShopItem* Item : ShopItemsDatas)
			Items.Add(*Item);

		OnUpdateItems.Broadcast(Items);
	}
}
