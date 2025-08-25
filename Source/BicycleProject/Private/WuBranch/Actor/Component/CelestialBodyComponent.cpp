// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/CelestialBodyComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UCelestialBodyComponent::UCelestialBodyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	Target = nullptr;
}


// Called when the game starts
void UCelestialBodyComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Time = 0.0f;
	Target = GetOwner()->GetComponentByClass<UDirectionalLightComponent>();
	OriginAngle = Target->GetRelativeRotation();
	//Y: Pitch, Z: Yaw, X: Roll
	ChangeSunAngle(START_ANGLE);
}


// Called every frame
void UCelestialBodyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	Time += DeltaTime;
	float Angle = CaculateSunAngle(Time);
	ChangeSunAngle(Angle);
	// 一日超えた
	/*if (sunAngle >= 360) {
		time -= 24 * 60 * 60;
	}*/
}

float UCelestialBodyComponent::GetSunAngle() const
{
	return SunAngle;
}

FMyTime UCelestialBodyComponent::GetTime()
{
	return ConvertAngleToTime(SunAngle);
}

float UCelestialBodyComponent::GetTimeRatio()
{
	return TIME_RATIO;
}

float UCelestialBodyComponent::CaculateSunAngle(float Sec) const
{
	// 180度 / 半日 => 180 / 12*60*60
	// 経過した時間　*　180度 / 半日　*　ゲームと現実の時間の比率　+　開始の角度
	return Sec * 180 / (12 * 60 * 60) * TIME_RATIO + START_ANGLE;
}

void UCelestialBodyComponent::ChangeSunAngle(float Angle)
{
	SunAngle = Angle;

	/*FString text = GetOwner()->GetActorNameOrLabel() + " : " + FString::Printf(TEXT("%f"), angle);
	UKismetSystemLibrary::PrintString(this, text, true, false, FColor::Green, 5.f);*/

	Target->SetWorldRotation(FRotator(Angle, OriginAngle.Yaw, OriginAngle.Roll));
	// GPUの負担を減らすために、夜になったら影を映らないにします
	int NowAngle = (int)Angle % 360;
	if (NowAngle > 180 && NowAngle < 360)
		Target->SetCastShadows(true);
	else
		Target->SetCastShadows(false);
	// UI更新
	OnUpdateTime.Broadcast(GetTime());
}

FMyTime UCelestialBodyComponent::ConvertAngleToTime(float Angle)
{
	// 時間調整
	// ゲーム中の昼：180~360 夜:0~180のため、先に昼を0~180に転換して
	Angle -= 180;
	Angle = (int)Angle % 360;
	/*FString text = "sun angle: " + FString::SanitizeFloat(angle);
	UKismetSystemLibrary::PrintString(this, text, true, false, FColor::Blue, 10.f);*/

	// 時間に変える
	// 昼：6時~18時　0度の時は６時
	// 24時間 = 360度 => 4分：1度
	float Hour = (float)Angle * 4 / 60 + 6;
	int Min = (Hour - (int)Hour) * 60;
	if (Hour >= 24)
		Hour -= 24;

	return FMyTime((int)Hour, Min);
}