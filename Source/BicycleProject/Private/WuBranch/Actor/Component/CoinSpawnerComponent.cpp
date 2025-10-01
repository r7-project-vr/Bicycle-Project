// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/CoinSpawnerComponent.h"
#include "WuBranch/Actor/Coin.h"
#include "Components/BoxComponent.h"
#include "UntakuBranch/Question.h"
#include "WuBranch/MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UCoinSpawnerComponent::UCoinSpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...

	Amount.Add(EQuestionLevel::Easy, 1);
	Amount.Add(EQuestionLevel::Normal, 1);
	Amount.Add(EQuestionLevel::Hard, 1);
}


// Called when the game starts
void UCoinSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	UMyGameInstance* GameInstance = GetWorld()->GetGameInstance<UMyGameInstance>();
	if (GameInstance)
	{
		GameInstance->OnUpdateCoinHeight.AddDynamic(this, &UCoinSpawnerComponent::UpdateCoinHeight);
		// 最初の高さを設定
		UpdateSpawnZoneHeight(GameInstance->GetCoinHeight());
	}
}


// Called every frame
//void UCoinSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UCoinSpawnerComponent::SetSeed(int Seed)
{
	RandomStream.Initialize(Seed);
}

void UCoinSpawnerComponent::Spawn(EQuestionLevel Level)
{
	if (Amount.Num() <= 0)
		return;

	int* Num = Amount.Find(Level);
	float* StartLocation = SpawnStartLocations.Find(Level);
	Spawn(*Num, *StartLocation);
}

void UCoinSpawnerComponent::DestroyCoin(ACoin* Coin)
{
	if (!Coins.Contains(Coin))
		return;

	Coins.Remove(Coin);
}

void UCoinSpawnerComponent::DestroyCoins()
{
	if (Coins.Num() <= 0)
		return;

	while (Coins.Num() > 0)
	{
		ACoin* Coin = Coins.Pop();
		Coin->Destroy();
	}
}

void UCoinSpawnerComponent::CancelDelegate()
{
	UMyGameInstance* GameInstance = GetWorld()->GetGameInstance<UMyGameInstance>();
	if (GameInstance)
	{
		GameInstance->OnUpdateCoinHeight.RemoveDynamic(this, &UCoinSpawnerComponent::UpdateCoinHeight);
	}
}

void UCoinSpawnerComponent::Spawn(int Num, float StartLocation)
{
	if (!CoinTemplate)
	{
		return;
	}
	
	for (int Index = 0; Index < Num; Index++)
	{
		for(int Times = 0; Times < SpawnTimes; Times++)
		{
			// 位置決定
			//FVector LocalLocation = RandomStream.RandPointInBox(SpawnZone);
			FVector LocalLocation = FVector(0.f, 0.f, SpawnZone.Min.Z);
			LocalLocation.X = StartLocation - Index * 2000.f - Times * 500.f;

			// Locationは相対位置なので、アクターの位置と回転に
			FVector WorldLocation = GetOwner()->GetActorTransform().TransformPosition(LocalLocation);

			ACoin* Coin = GetWorld()->SpawnActor<ACoin>(CoinTemplate, WorldLocation, FRotator::ZeroRotator);
			if (Coin)
			{
				Coin->Init(this);
				Coins.Add(Coin);
			}
		}
	}
}

void UCoinSpawnerComponent::UpdateCoinHeight(float NewHeight)
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player)
		return;

	//
	UpdateSpawnZoneHeight(NewHeight);
	
	// 生成したコインの高さも更新
	for (ACoin* Coin : Coins)
	{
		if (Coin)
		{
			Coin->ChangeBaseHeight(SpawnZone.Min.Z);
		}
	}
}

void UCoinSpawnerComponent::UpdateSpawnZoneHeight(float NewHeight)
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player)
		return;

	// プレイヤーの高さを取得
	float PlayerZ = Player->GetActorLocation().Z;
	// コインのコリジョーン高さを取得
	float CoinCollisionHieght = 0;
	if (CoinTemplate)
	{
		ACoin* DefaultCoin = CoinTemplate->GetDefaultObject<ACoin>();
		if (DefaultCoin && DefaultCoin->GetCollision())
		{
			CoinCollisionHieght = DefaultCoin->GetCollision()->GetScaledBoxExtent().Z;
		}
	}

	// 生成範囲の高さを更新(プレイヤー座標 + 腕の長さ + コインのコリジョーン高さ)
	SpawnZone.Max.Z = NewHeight + PlayerZ + CoinCollisionHieght;
	SpawnZone.Min.Z = NewHeight + PlayerZ + CoinCollisionHieght;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Update Spawn Zone Height: %f"), SpawnZone.Min.Z));
}

