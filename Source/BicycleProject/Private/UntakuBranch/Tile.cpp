// Fill out your copyright notice in the Description page of Project Settings.


#include "UntakuBranch/Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"
#include "UntakuBranch/TileManager.h"
#include "GameFramework/Character.h"
#include <WuBranch/Bike/BikeComponent.h>
#include <WuBranch/UI/QuestionUIActor.h>
#include <Kismet/GameplayStatics.h>
#include "WuBranch/Actor/Component/RandomFoliageSpawner.h"
#include "WuBranch/Actor/Component/EnvironmentalObjectComponent.h"
#include "WuBranch/Actor/Component/CoinSpawnerComponent.h"

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

	for (int Index = 0; Index < 34; Index++)
	{
		UEnvironmentalObjectComponent* Building = CreateDefaultSubobject<UEnvironmentalObjectComponent>(FName("Building_" + FString::FromInt(Index)));
		Building->SetupAttachment(RootComponent);
		Buildings.Add(Building);
	}

	// 2025.08.18 ウー start
	FoliageSpawner = CreateDefaultSubobject<URandomFoliageSpawner>("Foliage Spawner");
	FoliageSpawner->SetupAttachment(RootComponent);
	// 2025.08.18 ウー end

	// 2025.08.28 ウー start
	CoinSpawner = CreateDefaultSubobject<UCoinSpawnerComponent>("Coin Spawner");
	CoinSpawner->SetupAttachment(RootComponent);
	// 2025.08.28 ウー end
}

// 2025.08.01 ウー start
void ATile::SpawnMap(bool IsLeft)
{
	TileManager->SpawnNextMap(this, IsLeft);
}

void ATile::AdjustUI(FVector DeltaLocation, FRotator DeltaRotation)
{
	QuestionUI->AddActorLocalOffset(DeltaLocation);
	QuestionUI->AddActorLocalRotation(DeltaRotation);
}
// 2025.08.01 ウー end

// 2025.08.19 ウー start
void ATile::SpawnEnvironmentals(int Seed)
{
	// クイズUI
	CreateQuestionUI();
	// 建物
	for (UEnvironmentalObjectComponent* Building : Buildings)
	{
		Building->StartSpawnEnvironmentalObject();
	}
	// フォリッジ
	FoliageSpawner->SetSeed(Seed);
	FoliageSpawner->StartSpawnFoliage();
	// コイン
	CoinSpawner->SetSeed(Seed);
	CoinSpawner->Spawn();
}
// 2025.08.19 ウー end

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATile::OnOverlapBegin);
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
	QuestionUI = GetWorld()->SpawnActor<AQuestionUIActor>(QuestionActor, QuestionSpawnLocation->GetComponentTransform(), Params);
}

void ATile::DestroyAll()
{
	// 2025.08.19 ウー start
	// 問題UIを削除
	if(QuestionUI)
	{
		QuestionUI->Destroy();
	}

	// フォリッジを削除
	if (FoliageSpawner)
	{
		FoliageSpawner->RemoveFoliageInstances();
	}
	// 2025.08.19 ウー end

	// 建物を削除
	FVector MyLocation = GetActorLocation();
	const float MaxDistance = 15000.0f;
	TArray<AActor*> Actors;

	// 自身の上にある環境物を見つける
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticMeshActor::StaticClass(), Actors);
	Actors.RemoveAll([&](AActor* Actor) {
		if (!Actor)
			return true;

		// 範囲内
		FVector ObjectLocation = Actor->GetActorLocation();
		bool IsInRangeX = FMath::Abs(ObjectLocation.X - MyLocation.X) <= MaxDistance;
		bool IsInRangeY = FMath::Abs(ObjectLocation.Y - MyLocation.Y) <= MaxDistance;
		return !(IsInRangeX && IsInRangeY);
	});
	
	// 対象環境物を削除
	for (AActor* Actor : Actors)
	{
		Actor->Destroy();
	}
	// 自分も削除
	Destroy();
}
// 2025.08.01 ウー end