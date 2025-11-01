// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/CelebrateAnimal.h"
#include "Components/AudioComponent.h"

ACelebrateAnimal::ACelebrateAnimal()
	: AAnimal()
	, bIsCelebrating(false)
{
	CelebrateChirp = CreateDefaultSubobject<UAudioComponent>(TEXT("CelebrateChirp"));
	CelebrateChirp->SetupAttachment(RootComponent);
	CelebrateChirp->bAutoActivate = false;
}

void ACelebrateAnimal::BeginPlay()
{
	Super::BeginPlay();
	
	bIsCelebrating = false;
}

void ACelebrateAnimal::Celebrate()
{
	bIsCelebrating = true;
	if (CelebrateChirp && !CelebrateChirp->IsPlaying())
	{
		CelebrateChirp->Play();
	}
}

void ACelebrateAnimal::DecideBehavior()
{

}

void ACelebrateAnimal::Action(float DeltaTime)
{
	
}
