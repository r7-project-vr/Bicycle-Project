// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/DisplayComponent.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DirectionalLight.h"

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
	//UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), );
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
	if(Time.hour24)
}