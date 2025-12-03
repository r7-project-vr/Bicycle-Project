// Fill out your copyright notice in the Description page of Project Settings.


#include "tokuamaru/SpeedUI.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include <WuBranch/Bike/BikeComponent.h>

void USpeedUI::Init_Implementation(float init, float max,float danger,float safe,float avalage)
{
	currentY = init;
	maxSpeed = max;
	dangerSpeed = danger;
	safeSpeed = safe;
	avalageSpeed = avalage;
	PenaltyDuraction = 3.0f;
	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		if(UBikeComponent* BikeCom = Character->GetComponentByClass<UBikeComponent>())
			PenaltyDuraction = BikeCom->GetPenaltyDuration();
}

void USpeedUI::CounterChangeAninx_Implementation(float bikeSpeed)
{
	currentY = FMath::Clamp(bikeSpeed, 0.f, 1.f);
	nowSpeed = bikeSpeed;
}

void USpeedUI::SubTherd_Implementation(float delta)
{
	currentY -= (1.0f / PenaltyDuraction) * delta;
	if (currentY <= 0.0f) {
		currentY = 0.0f;
	}
}
