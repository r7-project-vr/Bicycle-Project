// Fill out your copyright notice in the Description page of Project Settings.


#include "tokuamaru/SpeedUI.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include <WuBranch/Bike/BikeComponent.h>

void USpeedUI::Init_Implementation(float init, float max,float danger,float safe,float avalage)
{
	nowSpeed = 0.0f;
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
	//currentY = FMath::Clamp(bikeSpeed, 0.f, 1.f);
	nowSpeed = FMath::Clamp(bikeSpeed, 0.f, 1.f);
}

void USpeedUI::DecayCurrentYOverDuration_Implementation(float delta)
{
	currentY -= (1.0f / PenaltyDuraction) * delta;
	nowSpeed = 0.0f;
	if (currentY <= 0.0f) {
		currentY = 0.0f;
	}
}

void USpeedUI::InterpolateCurrentYToTarget_Implementation(float delta)
{
	//増加割合が差によって増減
	currentY = FMath::FInterpTo(
		currentY,   // 現在値
		nowSpeed,    // 目標値
		delta,  // 経過時間
		5.0f        // 補間速度
	);

	//増加割合が一定
	//currentY = FMath::FInterpConstantTo(
	//	currentY,   // 現在値
	//	targetY,    // 目標値
	//	DeltaTime,  // 経過時間
	//	0.5f        // 移動速度 (1秒あたりに進む量)
	//);

}
