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

int UShopComponent::BuyItem(int ItemID)
{
	// アイテムを特定
	FShopItem* ShopItem = *(ShopItemsDatas.FindByPredicate([ItemID](const FShopItem* Item) {
		return Item->ID == ItemID;
		}));

	// 見つからない
	if (!ShopItem)
	{
		BuyFailed();
		return 1;
	}
	
	if (UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		//UnLockLimit分の写真を撮っているかチェック
		int32 CurrentPhotoPoints = GameInstance->GetAnimalPhotoPoint(ShopItem->ID);
		int32 RequiredPhotoPoints = ShopItem->UnLockLimit;
		
		if (CurrentPhotoPoints < RequiredPhotoPoints)
		{
			// アンロック条件未達成
			UE_LOG(LogTemp, Warning, TEXT("Animal ID %d is locked! Photos: %d / %d required"), 
				ShopItem->ID, CurrentPhotoPoints, RequiredPhotoPoints);
			BuyFailed();
			return 2;
		}
		
		// 金をチェック
		int CurrentCoins = GameInstance->GetTotalCoins();
		if (CurrentCoins < ShopItem->Price)
		{
			// 購入失敗　金足りない
			UE_LOG(LogTemp, Warning, TEXT("Not enough coins! Have: %d, Need: %d"), 
				CurrentCoins, ShopItem->Price);
			BuyFailed();
			return 3;
		}

		// 動物の数が最大に持っているか
		if (GameInstance->HasMaxAnimals())
		{
			UE_LOG(LogTemp, Warning, TEXT("Already have maximum number of animals!"));
			BuyFailed();
			return 4;
		}

		// 購入
		if (ShopItem->BuySuccSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ShopItem->BuySuccSound);
		}
		GameInstance->SetTotalCoins(CurrentCoins - ShopItem->Price);
		GameInstance->AddAnimal(ShopItem->ID);
		
		UE_LOG(LogTemp, Log, TEXT("Animal ID %d purchased successfully! Coins remaining: %d"), 
			ShopItem->ID, GameInstance->GetTotalCoins());
		
		return 0;
	}
	// 購入失敗
	BuyFailed();
	return 5;
}

TArray<FShopItem> UShopComponent::GetShopItems() const
{
	TArray<FShopItem> Items;
	for (const FShopItem* Item : ShopItemsDatas)
		Items.Add(*Item);
	return Items;
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
		OnUpdateItems.Broadcast(GetShopItems());
	}
}
