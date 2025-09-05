// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/CoinSpawnerComponent.h"
#include "WuBranch/Actor/Coin.h"
#include "UntakuBranch/Question.h"

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

void UCoinSpawnerComponent::Spawn(int Num)
{
	for (int Index = 0; Index < Num; Index++)
	{
		// 位置決定
		FVector LocalLocation = RandomStream.RandPointInBox(SpawnZone);

		// Locationは相対位置なので、アクターの位置と回転に
		FVector WorldLocation = GetOwner()->GetActorTransform().TransformPosition(LocalLocation);

		GetWorld()->SpawnActor<ACoin>(CoinTemplate, WorldLocation, FRotator::ZeroRotator);
	}
}

