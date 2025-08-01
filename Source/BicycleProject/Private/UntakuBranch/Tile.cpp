﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "UntakuBranch/Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "UntakuBranch/TileManager.h"
#include "GameFramework/Character.h"
#include <WuBranch/Bike/BikeComponent.h>
#include <WuBranch/UI/QuestionUIActor.h>

// Sets default values
ATile::ATile()
{
 	// Create and set TileMesh as RootComponent
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>("TileMesh");
	RootComponent = TileMesh;

	//Create Trigger and Hang on Root
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>("Trigger");
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->SetBoxExtent(FVector(15000.f, 15000.f, 200.f));
	TriggerVolume->SetRelativeLocation(FVector(0.f, 0.f, 99.5f));

	//Turn on the Overlap event
	TriggerVolume->SetGenerateOverlapEvents(true);
	
	QuestionSpawnLocation = CreateDefaultSubobject<UBoxComponent>("Question Spawn Location");
	QuestionSpawnLocation->SetupAttachment(RootComponent);
}

// 2025.08.01 ウー start
void ATile::SpawnMap(bool IsLeft)
{
	TileManager->SpawnNextMap(this, IsLeft);
}
// 2025.08.01 ウー end

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATile::OnOverlapBegin);
	
	CreateQuestionUI();
}

void ATile::OnOverlapBegin(UPrimitiveComponent* Overlapped,
	AActor* Other, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& Sweep)
{
	//Only player step in to Triggering
	if (Other->IsA(ACharacter::StaticClass()) && TileManager)
	{
		TileManager->OnPlayerSteppedOnTile(this);
	}
}

// 2025.08.01 ウー start
void ATile::CreateQuestionUI()
{
	if (!QuestionActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Question Actor did not set!"));
		return;
	}
		
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AQuestionUIActor* QuestionUI = GetWorld()->SpawnActor<AQuestionUIActor>(QuestionActor, QuestionSpawnLocation->GetComponentTransform(), Params);

}
// 2025.08.01 ウー end