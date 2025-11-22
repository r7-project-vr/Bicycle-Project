// Fill out your copyright notice in the Description page of Project Settings.


#include "tokuamaru/SpeedUI.h"

void USpeedUI::CounterChange_Implementation(float bikeSpeed)
{
	currentY = bikeSpeed / maxSpeed;
	nowSpeed = bikeSpeed;
	if (currentY >= 1.0f) {
		currentY = 1.0f;
		return;
	}
	if (currentY <= 0.0f) {
		currentY = 0.0f;
		return;
	}


}

void USpeedUI::Init_Implementation(float init, float max,float danger,float safe,float avalage)
{
	currentY = init;
	maxSpeed = max;
	dangerSpeed = danger;
	safeSpeed = safe;
	avalageSpeed = avalage;
}

void USpeedUI::CounterChangeAninx_Implementation(float bikeSpeed)
{
	currentY = bikeSpeed;
	nowSpeed = bikeSpeed;
	if (currentY >= 1.0f) {
		currentY = 1.0f;
		return;
	}
	if (currentY <= 0.0f) {
		currentY = 0.0f;
		return;
	}
}
