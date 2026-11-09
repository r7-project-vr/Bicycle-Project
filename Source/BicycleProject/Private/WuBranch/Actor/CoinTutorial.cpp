// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/CoinTutorial.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "WuBranch/Actor/Coin.h"
#include "WuBranch/MyGameInstance.h"
#include "Components/BoxComponent.h"

void ACoinTutorial::BeginPlay()
{
	Super::BeginPlay();

	FindPlayer();
}

void ACoinTutorial::HandleCoinCollected(ACoin* TouchedCoin)
{
	HideCoin();
	GotoNextStep();
}

void ACoinTutorial::PlaceCoin()
{
	if (Coin)
	{
		// コインの位置と方向を指定して更新する
		FVector Location;
		FRotator Forward;
		GetCoinLocationAndDir(Location, Forward);
		Coin->SetActorLocation(Location);
		Coin->SetActorRotation(Forward);
	}
	else
	{
		SpawnCoin();
	}

	ShowCoin();
}

void ACoinTutorial::HideCoin()
{
	if (!Coin)
		return;

	Coin->SetActorTickEnabled(false);
	Coin->SetActorHiddenInGame(true);
	Coin->GetCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACoinTutorial::FindPlayer()
{
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void ACoinTutorial::SpawnCoin()
{
	if (!CoinTemplate)
	{
		UE_LOG(LogTemp, Error, TEXT("Coin template is not set!"));
		return;
	}

	FVector Location;
	FRotator Forward;
	GetCoinLocationAndDir(Location, Forward);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Coin = GetWorld()->SpawnActor<ACoin>(CoinTemplate, Location, Forward, SpawnParams);

	if (Coin)
	{
		// 初期化
		Coin->Init(this);
	}
}

void ACoinTutorial::GetCoinLocationAndDir(FVector& OLocation, FRotator& ODir)
{
	if (!Player)
		FindPlayer();

	if (Player)
	{
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
		FVector PlayerForward = Player->GetActorForwardVector();
		FVector PlayerLocation = Player->GetActorLocation();
		OLocation = PlayerLocation + PlayerForward * CoinDistanceFromPlayer;

		float ArmLength = 0.f;
		UMyGameInstance* GameInstance = GetWorld()->GetGameInstance<UMyGameInstance>();
		if (GameInstance)
		{
			ArmLength = GameInstance->GetCoinHeight();
		}
		OLocation.Z = PlayerLocation.Z + ArmLength + CoinCollisionHieght;
		ODir = (-PlayerForward).Rotation();
	}
}

void ACoinTutorial::ShowCoin()
{
	if (!Coin)
		return;

	Coin->SetActorTickEnabled(true);
	Coin->SetActorHiddenInGame(false);
	Coin->GetCollision()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
