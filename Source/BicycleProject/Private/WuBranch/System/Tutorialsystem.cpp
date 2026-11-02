// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/System/Tutorialsystem.h"
#include "WuBranch/Struct/TutorialData.h"
#include "WuBranch/ProjectSettings/TutorialSettings.h"
#include "WuBranch/Bike/BikeCharacter.h"
#include "WuBranch/Actor/Component/TabletBaseComponent.h"
#include "WuBranch/Actor/TutorialActor.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Device/DeviceManager.h"

void UTutorialsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bIsTutorialInProgress = false;
	
	if (!LoadTutorialDatas())
	{
		return;
	}
}

void UTutorialsystem::FindAllTutorialActors()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATutorialActor::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (ATutorialActor* TutorialActor = Cast<ATutorialActor>(Actor))
		{
			TutorialActors.Add(TutorialActor->GetTutorialType(), TutorialActor);
		}
	}
}

bool UTutorialsystem::LoadTutorialDatas()
{
	UDataTable* Table = GetDefault<UTutorialSettings>()->TutorialDataTable.LoadSynchronous();

	if (!Table)
	{
		// なかった場合、デフォルトとして使用する
		Table = LoadObject<UDataTable>(nullptr, TEXT("/Game/WuBranch/DataTable/DT_TutorialDatas.DT_Tutorial"));
	}

	if (!Table || Table->GetRowStruct() != FTutorialData::StaticStruct())
	{
		UE_LOG(LogTemp, Warning, TEXT("TutorialSystem: 有効なFTutorialData Data Tableが見つからない"));
		return false;
	}

	TArray<FTutorialData*> Rows;
	Table->GetAllRows<FTutorialData>(TEXT("TutorialSystem"), Rows);
	for (const FTutorialData* Row : Rows)
	{
		TutorialDatas.Add(*Row);
	}
	return true;
}

void UTutorialsystem::OpenTutorial(ETutorialType TutorialType)
{	
	if (TutorialActors.Contains(TutorialType))
	{
		FTutorialData* TutorialData = TutorialDatas.FindByPredicate([&](const FTutorialData& Data) { return Data.Type == TutorialType; });
		if (!TutorialData)
			return;

		// プレイヤーの前に表せる
		ATutorialActor* Actor = TutorialActors[TutorialType];
		ABikeCharacter* Player = Cast<ABikeCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
		if (Player)
		{
			// プレイヤーの移動を先に止める
			if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
			{
				if(UDeviceManager* DeviceManager = GI->GetDeviceManager())
				{
					DeviceManager->DisableDefaultActions();
				}
			}
			Player->StopMove();

			FVector PlayerForward = Player->GetActorForwardVector();
			FVector Location = Player->GetActorLocation() + PlayerForward * TutorialData->Distance;
			Location.Z = Player->GetHeadHeight();
			Actor->SetActorLocation(Location);
			Actor->SetActorRotation((-PlayerForward).Rotation());

			bIsTutorialInProgress = true;
			Actor->StartTutorial();
			Player->EnableUIInteraction();
			Player->ShowGuideLine();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Tutorial type(%d) didnot set tutorial actor"), (int)TutorialType);
	}
}

void UTutorialsystem::FinishTutorial(ETutorialType TutorialType)
{
	if (TutorialActors.Contains(TutorialType))
	{
		bIsTutorialInProgress = false;
		// プレイヤーの移動を再開する
		if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
		{
			if (UDeviceManager* DeviceManager = GI->GetDeviceManager())
			{
				DeviceManager->EnableDefaultActions();
			}
		}

		TutorialActors[TutorialType]->FinishTutorial();
		ABikeCharacter* Player = Cast<ABikeCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
		if (Player)
		{
			Player->HideGuideLine();
			Player->DisableUIInteraction();
		}
	}
}

TArray<FTutorialData> UTutorialsystem::GetTutorialDatas() const
{
	return TutorialDatas;
}

void UTutorialsystem::SetNeedTutorial(bool bNeed)
{
	bNeedTutorial = bNeed;
}


