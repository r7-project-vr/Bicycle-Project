// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Pet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/GameplayStatics.h>
#include "WuBranch/DataAssets/AnimalDataAsset.h"
#include "WuBranch/Actor/Component/AnimalManagerComponent.h"

APet::APet()
	: AAnimal()
	, TargetChaseRate(0.5f)
	, IsChaseTarget(false)
	, ChaseLocation(FVector::ZeroVector)
	, StartChaseDistance(0.f)
	, GiveUpDistance(0.f)
	, IsPlayedChirp(false)
	, MinChaseSpeed(300.f)
	, MaxChaseSpeed(2000.f)
	, CatchUpRate(1.f)
	, RecallDistance(2500.f)
	, TargetSpeed(0.f)
	, PreviousTargetLocation(FVector::ZeroVector)
	, bHasPreviousTargetLocation(false)
	, RecallCoolTime(0.f)
{
	
}

void APet::Init(ACharacter* Target, UAnimalManagerComponent* Manager, UAnimalDataAsset* Data)
{
	Super::Init(Target, Manager, Data);

	if (Data)
	{
		FPetAnimalSettings PetSettings = Data->PetSettings;
		TargetChaseRate = PetSettings.TargetChaseRate;
		StartChaseDistance = PetSettings.StartChaseDistance;
		GiveUpDistance = PetSettings.GiveUpDistance;
		ChirpSE = Data->ChirpSE;
	}
}

void APet::LeaveBehind()
{
	if (!IsFollowing())
		return;

	// 鳴いてから諦める(次のActionでGiveUpする)
	PlayChirp();
	CurrentState = BehaviorState::GivingUp;
}

bool APet::IsFollowing() const
{
	return CurrentTarget != nullptr && CurrentState != BehaviorState::GivingUp;
}

void APet::BeginPlay()
{
	Super::BeginPlay();

	IsChaseTarget = false;
	ChaseLocation = GetActorLocation();
	TargetSpeed = 0.f;
	bHasPreviousTargetLocation = false;
	RecallCoolTime = 0.f;

}

void APet::DecideBehavior()
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	UpdateTargetSpeed(DeltaTime);

	FVector MyLocation = GetActorLocation();
	//
	if (IsChaseTarget)
		ChaseLocation = GetMyTargetLocation();
	else
		ChaseLocation = GetOffsetWorldLocation();
	// 追う目標との距離
	float Distance = FVector::DistXY(MyLocation, ChaseLocation);

	// プレイヤーが動いているか(オートプレイで運ばれている時も含む)
	bool bIsTargetMoving = TargetSpeed > 10.f;

	switch (CurrentState)
	{
	case BehaviorState::None:
		if(bIsTargetMoving)
		{
			// 次の目標を決める
			/*if (FMath::SRand() <= TargetChaseRate)
				IsChaseTarget = true;
			else
				IsChaseTarget = false;*/
			// 新しい偏移を設置
			ChangeOffset(GetNewOffset());
			CurrentState = BehaviorState::Chasing;
		}
		break;
	case BehaviorState::Chasing:
		// 目標に着いたら(プレイヤー: 200以下、偏移座標: 10以下)
		if ((IsChaseTarget && Distance <= 200.f) || (!IsChaseTarget && Distance <= 10.f))
		{
			// プレイヤーが動いている間は止まらずに次の位置へ走る
			if (bIsTargetMoving)
				ChangeOffset(GetNewOffset());
			else
				CurrentState = BehaviorState::None;
		}
		break;
	case BehaviorState::GivingUp:
		break;
	}

	// 障害物に引っかかる等で離れすぎたら、プレイヤーの後ろに呼び戻す
	RecallCoolTime -= DeltaTime;
	if (RecallCoolTime <= 0.f && FVector::DistXY(MyLocation, GetMyTargetLocation()) >= RecallDistance)
	{
		Recall();
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
	FVector Direction = (ChaseLocation - MyLocation).GetSafeNormal2D();

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("distance: %lf, StartChaseDistance: %lf"), Distance, StartChaseDistance));
	// プレイヤーの速さに合わせて走る、離れている分だけ速くして追いつく
	// (Moveは Speed * DeltaTime を入力にするため、フレームレートで速さが変わり最高600cm/sで自転車に置いていかれていた)
	GetCharacterMovement()->MaxWalkSpeed = FMath::Clamp(TargetSpeed + Distance * CatchUpRate, MinChaseSpeed, MaxChaseSpeed);

	// 向き変更
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), Direction.Rotation(), DeltaTime, 5.f));

	// 移動、速さはMaxWalkSpeedで決まる
	GetCharacterMovement()->AddInputVector(Direction);
}

void APet::UpdateTargetSpeed(float DeltaTime)
{
	FVector TargetLocation = GetMyTargetLocation();
	if (bHasPreviousTargetLocation && DeltaTime > KINDA_SMALL_NUMBER)
	{
		// ワープした時の飛びを抑えるため上限を付けて、なめらかにする
		float CurrentSpeed = FMath::Min(FVector::DistXY(TargetLocation, PreviousTargetLocation) / DeltaTime, MaxChaseSpeed);
		TargetSpeed = FMath::FInterpTo(TargetSpeed, CurrentSpeed, DeltaTime, 10.f);
	}
	PreviousTargetLocation = TargetLocation;
	bHasPreviousTargetLocation = true;
}

void APet::Recall()
{
	FVector GroundLocation;
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (!AnimalManager || !AnimalManager->FindLocationBehindTarget(Capsule->GetScaledCapsuleRadius(), GroundLocation))
	{
		// 置ける位置がなかったら少し待ってから試す
		RecallCoolTime = 1.f;
		return;
	}

	SetActorLocation(GroundLocation + FVector(0, 0, Capsule->GetScaledCapsuleHalfHeight()), false, nullptr, ETeleportType::TeleportPhysics);
	GetCharacterMovement()->StopMovementImmediately();
	ChangeOffset(GetNewOffset());
	CurrentState = BehaviorState::Chasing;
}

FVector APet::GetNewOffset()
{
	// 今プレイヤーの正面は0度になっている
	// ランダムな角度(30度 ~ 120度, 240度 ~ 330度)
	float Angles[4] = { (float)1 / 6, (float)5 / 12, (float)19 / 12, (float)11 / 6 };
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