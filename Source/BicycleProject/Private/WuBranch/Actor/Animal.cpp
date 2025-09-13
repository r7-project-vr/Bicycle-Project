// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Animal.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/Bike/BikeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WuBranch/Actor/Component/AnimalManagerComponent.h"

// Sets default values
AAnimal::AAnimal()
	: CurrentState(BehaviorState::None)
	, IsChaseTarget(false)
	, StartChaseDistance(100.f)
	, Speed(10.f)
	, IsPaused(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->BodyInstance.bLockXRotation = true;
	GetCapsuleComponent()->BodyInstance.bLockYRotation = true;
}

// Called when the game starts or when spawned
void AAnimal::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentState = BehaviorState::None;
	ChaseLocation = GetActorLocation();
	IsChaseTarget = false;
	IsPaused = false;
}

// Called every frame
void AAnimal::Tick(float DeltaTime)
{
	if(IsPaused)
		return;

	Super::Tick(DeltaTime);

	if (CurrentTarget.IsNull())
	{
		CurrentState = BehaviorState::None;
		return;
	}

	DecideBehavior();
	Action(DeltaTime);
}

void AAnimal::Pause_Implementation()
{
	IsPaused = true;
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
}

void AAnimal::ReStart_Implementation()
{
	IsPaused = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

bool AAnimal::IsPause_Implementation()
{
	return IsPaused;
}

float AAnimal::GetCurrentSpeed() const
{
	return GetCharacterMovement()->Velocity.Length();
}

float AAnimal::GetChaseDistance() const
{
	return StartChaseDistance;
}

void AAnimal::Init(ACharacter* Target, UAnimalManagerComponent* Manager)
{
	CurrentTarget = Target;
	AnimalManager = Manager;
}

void AAnimal::ChangeOffset(FVector Offset)
{
	RelativeOffset = Offset;
	ChaseLocation = GetCurrentOffsetLocation();
}

void AAnimal::DecideBehavior()
{
	FVector MyLocation = GetActorLocation();
	//
	if(IsChaseTarget)
		ChaseLocation = GetTargetLocation();
	else
		ChaseLocation = GetCurrentOffsetLocation();
	// 追う目標との距離
	float Distance = FVector::DistXY(MyLocation, ChaseLocation);
	
	switch (CurrentState)
	{
	case BehaviorState::None:
		if (Distance >= GiveUpDistance)
			CurrentState = BehaviorState::GivingUp;
		else if (Distance >= StartChaseDistance)
		{
			if (FMath::SRand() <= TargetChaseRate)
				IsChaseTarget = true;
			else
				IsChaseTarget = false;
			CurrentState = BehaviorState::Chasing;
		}
		break;
	case BehaviorState::Chasing:
		// 一定以上距離を離れたら追うのをやめる
		if (Distance >= GiveUpDistance)
			CurrentState = BehaviorState::GivingUp;
		// 目標に着いたら(プレイヤー: 200以下、偏移座標: 20以下)
		else if ((IsChaseTarget && Distance <= 200.f) || (!IsChaseTarget && Distance <= 20.f))
		{
			// 新しい偏移を設置
			ChangeOffset(GetNewOffset());
			CurrentState = BehaviorState::None;
		}
		break;
	case BehaviorState::GivingUp:
		break;
	}
}

void AAnimal::Action(float DeltaTime)
{
	switch (CurrentState)
	{
	case BehaviorState::None:
		break;
	case BehaviorState::Chasing:
		Chase(DeltaTime);
		break;
	case BehaviorState::GivingUp:
		GiveUp();
		break;
	}
}

FVector AAnimal::GetTargetLocation()
{
	ACharacter* Player = CurrentTarget.Get();
	return Player->GetActorLocation();
}

FVector AAnimal::GetCurrentOffsetLocation()
{
	ACharacter* Player = CurrentTarget.Get();
	FVector PlayerLocation = Player->GetActorLocation();
	FRotator PlayerRotation = Player->GetActorRotation();
	// ワールド上偏移した位置はプレイヤー位置 + 偏移量 * プレイヤーの角度
	return PlayerLocation + PlayerRotation.RotateVector(RelativeOffset);
}

void AAnimal::Chase(float DeltaTime)
{
	FVector MyLocation = GetActorLocation();
	// 目標との距離
	float Distance = FVector::DistXY(MyLocation, ChaseLocation);
	// 方向
	FVector Direction = (ChaseLocation - MyLocation).GetSafeNormal();

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("distance: %lf, StartChaseDistance: %lf"), Distance, StartChaseDistance));
	Move(DeltaTime, Direction);

	// 諦める前に鳴く, 距離の8割
	if (Distance >= GiveUpDistance * 0.8)
	{
		if (ChirpSE)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ChirpSE, GetActorLocation());
		}
	}
}

void AAnimal::Move(float DeltaTime, FVector Direction)
{
	// 水平面のみ
	Direction.Z = 0;
	// 向き変更
	FRotator TargetRotation = Direction.Rotation();
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.f));

	// 移動
	GetCharacterMovement()->AddInputVector(Direction.GetSafeNormal() * Speed * DeltaTime);
}

void AAnimal::GiveUp()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("%s distance: %lf"), *GetFName().ToString(), TotalDistance));
	AnimalManager->ReduceAnimal(this);
	CurrentTarget = nullptr;
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

FVector AAnimal::GetNewOffset()
{
	// 今プレイヤーの正面は0度になっている
	// ランダムな角度(30度 ~ 120度, 240度 ~ 330度)
	float Angles[4] = { (float)1 / 4 , (float) 2 / 3, (float) 4 / 3, (float) 7 / 4 };
	// 左右をランダムで決める、左：［0, 0.5), 右: [0.5, 1)
	int Side = FMath::RoundToInt(FMath::SRand());
	float StartAngle = Angles[Side * 2];
	float EndAngle = Angles[Side * 2 + 1];
	float Angle = FMath::FRandRange(StartAngle * PI, EndAngle * PI);

	// ランダムな距離（0～追い始まる距離の８割）、均等分布のため sqrt を使う
	float Distance = FMath::Sqrt(FMath::RandRange(0.0f, 1.0f)) * StartChaseDistance * 0.8;

	return FVector(FMath::Cos(Angle) * Distance, FMath::Sin(Angle) * Distance, 0);
}
