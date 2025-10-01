// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/DisplayComponent.h"
#include "WuBranch/Actor/Celestial.h"
#include "Kismet/GameplayStatics.h"
#include <WuBranch/Actor/Component/OrbitalRevolutionComponent.h>

// Sets default values for this component's properties
UDisplayComponent::UDisplayComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDisplayComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACelestial::StaticClass(), FName("Sun"), Actors);
	if (Actors.Num() > 0)
	{
		UOrbitalRevolutionComponent* Celestial = Actors[0]->FindComponentByClass<UOrbitalRevolutionComponent>();
		if (Celestial)
		{
			Celestial->OnUpdateTime.AddDynamic(this, &UDisplayComponent::UpdateTime);
		}
	}
}

// Called every frame
//void UDisplayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}


void UDisplayComponent::UpdateTime(FMyTime Time)
{
	if (Time.hour24 == ShowTime.hour24 && Time.min >= ShowTime.min)
	{
		// 表示
		GetOwner()->SetActorHiddenInGame(false);
	}
	else if (Time.hour24 == HideTime.hour24 && Time.min >= HideTime.min)
	{
		// 非表示
		GetOwner()->SetActorHiddenInGame(true);
	}
}