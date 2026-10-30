// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/PhotoTutorial.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/Bike/BikeCharacter.h"
#include "WuBranch/Bike/BikeComponent.h"
#include <UntakuBranch/Tile.h>
#include "WuBranch/Actor/Component/WildAnimalManagerComponent.h"
#include "WuBranch/Actor/TutorialAnimal.h"
#include "WuBranch/Actor/TutorialIndicator.h"

void APhotoTutorial::BeginPlay()
{
	Super::BeginPlay();

	FindPlayer();
	FindIndicator();
}

void APhotoTutorial::ShowTutorialAnimal()
{
	// すでに生成された場合
	if (TutorialAnimal)
	{
		// チュートリアル動物を移動する
		FVector AnimalLocation;
		FRotator AnimalForward;
		GetAnimalLocationAndDir(AnimalLocation, AnimalForward);
		TutorialAnimal->SetActorLocation(AnimalLocation);
		TutorialAnimal->SetActorRotation(AnimalForward);
		TutorialAnimal->SetActorTickEnabled(true);
		TutorialAnimal->SetActorHiddenInGame(false);
	}
	else
	{
		SpawnTutorialAnimal();
	}

	// フェードイン
	TutorialAnimal->FadeIn();
}

void APhotoTutorial::HideTutorialAnimal()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Hide Tutorial Animal"));
	if (TutorialAnimal)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Tutorial Animal Fade out start"));
		TutorialAnimal->FadeOut();
	}
}

void APhotoTutorial::ShowIndicator(FVector Target)
{
	if (Indicator)
	{
		Indicator->SetTarget(Target);
		Indicator->ShowArrow();
	}
}

void APhotoTutorial::HideIndicator()
{
	if (Indicator)
	{
		Indicator->HideArrow();
	}
}

void APhotoTutorial::FindPlayer()
{
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void APhotoTutorial::FindIndicator()
{
	 AActor* Target = UGameplayStatics::GetActorOfClass(GetWorld(), ATutorialIndicator::StaticClass());
	 if (ATutorialIndicator* TutorialIndicator = Cast<ATutorialIndicator>(Target))
	 {
		 Indicator = TutorialIndicator;
	 }
}

//UWildAnimalManagerComponent* APhotoTutorial::GetWildAnimalManagerFromCurrentTile(ACharacter* Character)
//{
//	if (ABikeCharacter* BikeCharacter = Cast<ABikeCharacter>(Character))
//	{
//		if (UBikeComponent* BikeComponent = BikeCharacter->GetBikeComponent())
//		{
//			// 現在立っているTileからWildAnimalManagerを取得
//			if (ATile* Map = BikeComponent->FindCurrentTile())
//			{
//				return Map->GetWildAnimalManager();
//			}
//		}
//	}
//	return nullptr;
//}

void APhotoTutorial::SpawnTutorialAnimal()
{
	if (!AnimalTemplate || !AnimalData)
	{
		UE_LOG(LogTemp, Error, TEXT("Wild Animal's template and data is not set!"));
		return;
	}

	if (!Player)
		FindPlayer();
	
	if (Player)
	{
		FVector AnimalLocation;
		FRotator AnimalForward;
		GetAnimalLocationAndDir(AnimalLocation, AnimalForward);
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		TutorialAnimal = GetWorld()->SpawnActor<ATutorialAnimal>(AnimalTemplate, AnimalLocation, AnimalForward, SpawnParams);

		if (TutorialAnimal)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Init tutorial animal"));
			// プレイヤーキャラクターで初期化
			TutorialAnimal->Init(Player, nullptr, AnimalData);
			TutorialAnimal->GetSelfMaterial();

			// 
			TutorialAnimal->OnFadeOutComplete.AddDynamic(this, &APhotoTutorial::OnTutorialAnimalFadeOutComplete);
		}
	}
}

void APhotoTutorial::GetAnimalLocationAndDir(FVector& OLocation, FRotator& ODir)
{
	if (!Player)
		FindPlayer();

	if (Player)
	{
		FVector PlayerForward = Player->GetActorForwardVector();
		PlayerForward = PlayerForward.RotateAngleAxis(AnimalDirection, FVector::UpVector);
		FVector PlayerLocation = Player->GetActorLocation();
		OLocation = PlayerLocation + PlayerForward * AnimalDistanceFromPlayer;
		ODir = (-PlayerForward).Rotation();
	}
}

void APhotoTutorial::OnTutorialAnimalFadeOutComplete()
{
	FVector Location = TutorialAnimal->GetActorLocation();
	Location.Z = HightWhenHide;
	TutorialAnimal->SetActorLocation(Location);
	TutorialAnimal->SetActorTickEnabled(false);
	TutorialAnimal->SetActorHiddenInGame(true);
}
