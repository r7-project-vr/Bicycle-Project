// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Firework.h"
#include "NiagaraComponent.h"

// Sets default values
AFirework::AFirework()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	_fireworkEffect = CreateDefaultSubobject<UNiagaraComponent>(FName("Firework Effect"));
	_fireworkEffect->SetupAttachment(RootComponent);
	_fireworkEffect->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AFirework::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFirework::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFirework::Fire()
{
	if (!_fireworkEffect->IsActive())
	{
		_fireworkEffect->Activate(true);
	}
}

