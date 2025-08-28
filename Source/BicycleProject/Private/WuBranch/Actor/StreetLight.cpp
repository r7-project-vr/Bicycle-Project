// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/StreetLight.h"
#include "Components/PointLightComponent.h"
#include "WuBranch/Struct/MyTime.h"

// Sets default values
AStreetLight::AStreetLight()
	: Intensity(10.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AStreetLight::BeginPlay()
{
	Super::BeginPlay();
	
	TurnOff();
}

// Called every frame
//void AStreetLight::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AStreetLight::UpdateTime(FMyTime NewTime)
{
	// 点ける
	if (NewTime.hour24 == TurnOnTime.hour24 && NewTime.min >= TurnOnTime.min)
		TurnOn();
	// 消す
	else if (NewTime.hour24 == TurnOffTime.hour24 && NewTime.min >= TurnOffTime.min)
		TurnOff();
}

void AStreetLight::TurnOff()
{
	Light->SetIntensity(0.f);
}