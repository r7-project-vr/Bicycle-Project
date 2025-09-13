// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/CoinSpawnerComponent.h"
#include "WuBranch/Actor/Coin.h"
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
	Spawn(*Num);
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

void UCoinSpawnerComponent::Spawn(int Num)
{
	SpawnZone.Min.Z = 358.f;
	SpawnZone.Max.Z = 358.f;
	for (int Index = 0; Index < Num; Index++)
	{
		// 位置決定
		FVector LocalLocation = RandomStream.RandPointInBox(SpawnZone);

		// Locationは相対位置なので、アクターの位置と回転に
		FVector WorldLocation = GetOwner()->GetActorTransform().TransformPosition(LocalLocation);

		ACoin* Coin = GetWorld()->SpawnActor<ACoin>(CoinTemplate, WorldLocation, FRotator::ZeroRotator);
		Coin->Init(this);
		Coins.Add(Coin);
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

	float PlayerZ = Player->GetActorLocation().Z;

	// 生成範囲の高さを更新
	SpawnZone.Max.Z = NewHeight + PlayerZ;
	SpawnZone.Min.Z = NewHeight + PlayerZ;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Update Spawn Zone Height: %f"), SpawnZone.Min.Z));
}

