// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/WildAnimal.h"
#include <Kismet/GameplayStatics.h>
#include "WuBranch/DataAssets/AnimalDataAsset.h"

AWildAnimal::AWildAnimal()
	: AAnimal()
	, Face(FVector::ZeroVector)
{
}

void AWildAnimal::Init(ACharacter* Target, UAnimalManagerComponent* Manager, UAnimalDataAsset* Data)
{
	if (Data)
	{
		Super::Init(Target, Manager, Data);
		FWildAnimalSettings WildSettings = Data->WildSettings;
		RotationSpeed = WildSettings.RotationSpeed;
		ChirpSE = Data->ChirpSE;
	}
	else
	{
		Super::Init(Target, Manager);
	}
}

void AWildAnimal::BeginPlay()
{
	Super::BeginPlay();

	Face = GetActorForwardVector().GetSafeNormal2D();
}

void AWildAnimal::DecideBehavior()
{
	// 現在アクターが持っているコントローラーを取得
	AAIController* CurrentAIC = Cast<AAIController>(GetController());

	// コントローラーが存在し、かつ指定したクラス型であるか判定
	if (CurrentAIC && CurrentAIC->IsA(WildAnimalControllerClass)) {

	}
	else {
		NeedChirp = IsTargetPassed();
	}
}

void AWildAnimal::Action(float DeltaTime)
{
	// 現在アクターが持っているコントローラーを取得
	AAIController* CurrentAIC = Cast<AAIController>(GetController());

	// コントローラーが存在し、かつ指定したクラス型であるか判定
	if (CurrentAIC && CurrentAIC->IsA(WildAnimalControllerClass)) {

	}
	else {
		if (CurrentState == BehaviorState::None)
		{
			if (ChirpSE && NeedChirp)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ChirpSE, GetActorLocation());
			}
		}
		StareAtTarget(DeltaTime);
	}
}

bool AWildAnimal::IsTargetPassed()
{
	if(!CurrentTarget)
		return false;

	bool Result = false;

	// 今回の内積
	ACharacter* Player = CurrentTarget.Get();
	FVector PlayerLocation = Player->GetActorLocation();
	FVector CurrentCross = FVector::CrossProduct(Face, (PlayerLocation - GetActorLocation()).GetSafeNormal2D());
	
	// 前回と今回の結果を掛け算して負になったら通り過ぎた
	if (PreviousCross.Z * CurrentCross.Z < 0.f)
		Result = true;

	// 今回の結果を保存
	if(PreviousCross != CurrentCross)
		PreviousCross = CurrentCross;

	return Result;
}

void AWildAnimal::StareAtTarget(float DeltaTime)
{
	if (!CurrentTarget)
		return;
	
	ACharacter* Player = CurrentTarget.Get();

	// 自身の向きとプレイヤーの角度の計算
	FVector PlayerLocation = Player->GetActorLocation();
	FVector Direction = (PlayerLocation - GetActorLocation());
	Direction.Z = 0.f;
	Direction = Direction.GetSafeNormal();
	float Dot = FVector::DotProduct(Face, Direction);

	// +-60度以内なら目線で追う
	// cos(45度) = 0.707
	if (Dot >= 0.707f)
	{
	    FRotator RotationPerFrame = FMath::RInterpTo(GetActorRotation(), Direction.Rotation(), DeltaTime, RotationSpeed);
		SetActorRotation(RotationPerFrame);
	}
}
