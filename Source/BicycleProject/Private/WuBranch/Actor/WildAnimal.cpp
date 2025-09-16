// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/WildAnimal.h"
#include <Kismet/GameplayStatics.h>

AWildAnimal::AWildAnimal()
	: AAnimal()
	, Face(FVector::ZeroVector)
{
}

void AWildAnimal::BeginPlay()
{
	Super::BeginPlay();

	Face = GetActorForwardVector().GetSafeNormal2D();
}

void AWildAnimal::DecideBehavior()
{
	NeedChirp = IsTargetPassed();
}

void AWildAnimal::Action(float DeltaTime)
{
	if (CurrentState == BehaviorState::None)
	{
		if (ChirpSE && NeedChirp)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ChirpSE, GetActorLocation());
		}
	}

	StareAtTarget();
}

bool AWildAnimal::IsTargetPassed()
{
	if(CurrentTarget.IsNull())
		return false;

	bool Result = false;

	// 今回の内積
	ACharacter* Player = CurrentTarget.Get();
	FVector PlayerLocation = Player->GetActorLocation();
	float CurrentDot = FVector::DotProduct(Face, (PlayerLocation - GetActorLocation()).GetSafeNormal());
	
	// 前回と今回の結果を掛け算して負になったら通り過ぎた
	if (PreviousDot * CurrentDot < 0.f)
		Result = true;

	// 今回の結果を保存
	PreviousDot = CurrentDot;

	return Result;
}

void AWildAnimal::StareAtTarget()
{
	if (CurrentTarget.IsNull())
		return;
	
	ACharacter* Player = CurrentTarget.Get();

	// 自身の向きとプレイヤーの角度の計算
	FVector PlayerLocation = Player->GetActorLocation();
	FVector Direction = (PlayerLocation - GetActorLocation());
	Direction.Z = 0.f;
	Direction.GetSafeNormal();
	float Dot = FVector::DotProduct(Face, Direction);
	
	// +-60度以内なら目線で追う
	if (Dot >= 0.5)
	{
		SetActorRotation(Direction.Rotation());
	}
}
