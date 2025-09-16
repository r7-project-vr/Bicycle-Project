// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Celestial.h"
#include "WuBranch/Actor/Component/OrbitalRevolutionComponent.h"
#include "Components/DirectionalLightComponent.h"

// Sets default values
ACelestial::ACelestial()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Light = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Light"));
	RootComponent = Light;

	Revolution = CreateDefaultSubobject<UOrbitalRevolutionComponent>(TEXT("Orbital Revolution"));
}

// Called when the game starts or when spawned
void ACelestial::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACelestial::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACelestial::Pause_Implementation()
{
	IsPause = true;
	if (Revolution)
		Revolution->Pause();
}

void ACelestial::ReStart_Implementation()
{
	IsPause = false;
	if (Revolution)
		Revolution->ReStart();
}

bool ACelestial::IsPause_Implementation()
{
	return IsPause;
}

