// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Pet.h"
#include <Kismet/GameplayStatics.h>

APet::APet()
	: AAnimal()
	, TargetChaseRate(0.5f)
	, IsChaseTarget(false)
	, ChaseLocation(FVector::ZeroVector)
	, StartChaseDistance(0.f)
	, GiveUpDistance(0.f)
	, IsPlayedChirp(false)
{
	
}

void APet::BeginPlay()
{
	Super::BeginPlay();

	IsChaseTarget = false;
	ChaseLocation = GetActorLocation();
}

void APet::DecideBehavior()
{
	FVector MyLocation = GetActorLocation();
	//
	if (IsChaseTarget)
		ChaseLocation = GetTargetLocation();
	else
		ChaseLocation = GetOffsetWorldLocation();
	// 追う目標との距離
	float Distance = FVector::DistXY(MyLocation, ChaseLocation);

	switch (CurrentState)
	{
	case BehaviorState::None:
		if (Distance >= GiveUpDistance)
			CurrentState = BehaviorState::GivingUp;
		else if (Distance >= StartChaseDistance)
		{
			// 次の目標を決める
			if (FMath::SRand() <= TargetChaseRate)
				IsChaseTarget = true;
			else
				IsChaseTarget = false;
			// 新しい偏移を設置
			ChangeOffset(GetNewOffset());
			CurrentState = BehaviorState::Chasing;
		}
		break;
	case BehaviorState::Chasing:
		// 一定以上距離を離れたら追うのをやめる
		if (Distance >= GiveUpDistance)
			CurrentState = BehaviorState::GivingUp;
		// 目標に着いたら(プレイヤー: 200以下、偏移座標: 10以下)
		else if ((IsChaseTarget && Distance <= 200.f) || (!IsChaseTarget && Distance <= 10.f))
			CurrentState = BehaviorState::None;
		break;
	case BehaviorState::GivingUp:
		break;
	}
}

void APet::Action(float DeltaTime)
{
	switch (CurrentState)
	{
	case BehaviorState::None:
		IsPlayedChirp = false;
		break;
	case BehaviorState::Chasing:
		Chase(DeltaTime);
		break;
	case BehaviorState::GivingUp:
		GiveUp();
		break;
	}
}

void APet::Chase(float DeltaTime)
{
	FVector MyLocation = GetActorLocation();
	// 目標との距離
	float Distance = FVector::DistXY(MyLocation, ChaseLocation);
	// 方向
	FVector Direction = (ChaseLocation - MyLocation).GetSafeNormal();

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("distance: %lf, StartChaseDistance: %lf"), Distance, StartChaseDistance));
	Move(DeltaTime, Direction);

	// 諦める前に鳴く, 諦める距離の8割
	if (!IsPlayedChirp && Distance >= GiveUpDistance * 0.8)
	{
		PlayChirp();
	}
}

FVector APet::GetNewOffset()
{
	// 今プレイヤーの正面は0度になっている
	// ランダムな角度(30度 ~ 120度, 240度 ~ 330度)
	float Angles[4] = { (float)1 / 4 , (float)2 / 3, (float)4 / 3, (float)7 / 4 };
	// 左右をランダムで決める、左：［0, 0.5), 右: [0.5, 1)
	int Side = FMath::RoundToInt(FMath::SRand());
	float StartAngle = Angles[Side * 2];
	float EndAngle = Angles[Side * 2 + 1];
	float Angle = FMath::FRandRange(StartAngle * PI, EndAngle * PI);

	// ランダムな距離（0～800,道路の幅さ）、均等分布のため sqrt を使う
	float Distance = FMath::Sqrt(FMath::RandRange(0.0f, 1.0f)) * 800;
	// 最小距離200cm
	Distance = Distance < 200.f ? 200.f : Distance;

	return FVector(FMath::Cos(Angle) * Distance, FMath::Sin(Angle) * Distance, 0);
}

void APet::PlayChirp()
{
	if (ChirpSE)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ChirpSE, GetActorLocation());
		IsPlayedChirp = true;
	}
}

//void AAnimal::CountDownChangeOffset(float DeltaTime)
//{
//	TimeCnt -= DeltaTime;
//
//	// 新しい偏移量を決める
//	if (TimeCnt <= 0.f)
//	{
//		// 新しい偏移を設置
//		ChangeOffset(GetNewOffset());
//		// 次の変更時間を更新
//		TimeCnt = GetNextChangeOffsetTime();
//	}
//}

//float AAnimal::GetNextChangeOffsetTime()
//{
//	return ChangeOffsetTime + FMath::RandRange(-RandomDeviation, RandomDeviation);
//}