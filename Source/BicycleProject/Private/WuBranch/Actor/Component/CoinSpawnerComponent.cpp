// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/CoinSpawnerComponent.h"
#include "WuBranch/Actor/Coin.h"

// Sets default values for this component's properties
UCoinSpawnerComponent::UCoinSpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
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

void UCoinSpawnerComponent::Spawn()
{
	for (int Index = 0; Index < Amount; Index++)
	{
		// 位置決定
		FVector Location = RandomStream.RandPointInBox(SpawnZone);

		GetWorld()->SpawnActor<ACoin>(CoinTemplate, Location, FRotator::ZeroRotator);
	}
}

void UCoinSpawnerComponent::Spawn(int Num)
{
	for (int Index = 0; Index < Num; Index++)
	{
		// 位置決定
		FVector Location = RandomStream.RandPointInBox(SpawnZone);

		GetWorld()->SpawnActor<ACoin>(CoinTemplate, Location, FRotator::ZeroRotator);
	}
}

