// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/HeightTesterComponent.h"
#include "WuBranch/MyGameInstance.h"
#include "Components/SphereComponent.h"
#include <Components/WidgetComponent.h>
#include <WuBranch/UI/SettingWidget.h>

// Sets default values for this component's properties
UHeightTesterComponent::UHeightTesterComponent()
	: IsUseRight(true)
	, IsUseLeft(false)
	, TotalRecalibrationCount(10)
	, CollectionInterval(0.5f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHeightTesterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GameInstance = GetWorld()->GetGameInstance<UMyGameInstance>();
	LeftHand = GetOwner()->FindComponentByTag<USphereComponent>(FName("LeftHand"));
	RightHand = GetOwner()->FindComponentByTag<USphereComponent>(FName("RightHand"));

	RecalibrationOneTime.BindUFunction(this, "DoOneRecalibration");
	RecalibrationParas.bLoop = true;
	RecalibrationParas.FirstDelay = 0.5f;
}


// Called every frame
//void UHeightTesterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UHeightTesterComponent::StartRecalibration()
{
	Results.Empty();
	GetWorld()->GetTimerManager().SetTimer(RecalibrationTimer, RecalibrationOneTime, CollectionInterval, RecalibrationParas);
}

void UHeightTesterComponent::SetUseHand(bool Left, bool Right)
{
	IsUseRight = Right;
	IsUseLeft = Left;
}

void UHeightTesterComponent::GetUsedHand(bool& OutLeft, bool& OutRight)
{
	OutLeft = IsUseLeft;
	OutRight = IsUseRight;
}

int UHeightTesterComponent::GetTotalRecalibrationCount() const
{
	return TotalRecalibrationCount;
}

void UHeightTesterComponent::DoOneRecalibration()
{
	if (!LeftHand && !RightHand)
	{
		// 強制終了
		FinishRecalibration();
		return;
	}

	if (IsUseLeft)
	{
		FVector LeftHandLocation = LeftHand->GetComponentLocation();
		Results.Add(LeftHandLocation.Z);
	}

	if (IsUseRight)
	{
		FVector RightHandLocation = RightHand->GetComponentLocation();
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("RightHandLocation.Z: %lf"), RightHandLocation.Z));
		Results.Add(RightHandLocation.Z);
	}

	NotifyCalibrationProgress(Results.Num());

	if (Results.Num() >= TotalRecalibrationCount)
	{
		FinishRecalibration();
	}
}

void UHeightTesterComponent::FinishRecalibration()
{
	// タイマー停止
	GetWorld()->GetTimerManager().ClearTimer(RecalibrationTimer);
	// 結果を計算
	float Avg = CaculateAvg();

	// 腕の長さを計算
	float PlayerHiehgt = Avg - GetOwner()->GetActorLocation().Z;

	UseResultInGame(PlayerHiehgt);
	NotifyRecalibrationCompleted();
}

float UHeightTesterComponent::CaculateAvg()
{
	float Sum = 0.f;
	for (float Result : Results)
	{
		Sum += Result;
	}
	return Sum / Results.Num();
}

void UHeightTesterComponent::UseResultInGame(float Avg)
{
	if (!GameInstance)
		return;
	
	GameInstance->SetCoinHeight(Avg);
}

void UHeightTesterComponent::NotifyCalibrationProgress(int CurrentProgress)
{
	if (OnCalibrationProgress.IsBound())
	{
		OnCalibrationProgress.Broadcast(CurrentProgress);
	}
}

void UHeightTesterComponent::NotifyRecalibrationCompleted()
{
	if (OnRecalibrationCompleted.IsBound())
	{
		OnRecalibrationCompleted.Broadcast();
	}
}

