// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Animal.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/Bike/BikeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WuBranch/Actor/Component/AnimalManagerComponent.h"

// Sets default values
AAnimal::AAnimal()
	: StartChaseDistance(100.f)
	, Speed(10.f)
	, CanMoreChase(false)
	, ChangeOffsetTime(180.f)
	, RandomDeviation(10.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAnimal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentTarget.IsNull())
		return;

	if(!GiveUp())
		Chase(DeltaTime);

	CountDownChangeOffset(DeltaTime);
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
}

void AAnimal::Chase(float DeltaTime)
{
	ACharacter* Player = CurrentTarget.Get();

	FVector PlayerLocation = Player->GetActorLocation();
	FRotator PlayerRotation = Player->GetActorRotation();
	FVector MyLocation = GetActorLocation();
	// 目標位置、プレイヤー位置 + 相対位置 * プレイヤーの角度
	FVector TargetLocation = PlayerLocation + PlayerRotation.RotateVector(RelativeOffset);
	// 目標との距離
	float Distance = FVector::DistXY(MyLocation, TargetLocation);

	// 方向
	FVector Direction = (TargetLocation - MyLocation).GetSafeNormal();

	// 一定以上の距離を離れたら追う
	if (Distance >= StartChaseDistance)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("distance: %lf, StartChaseDistance: %lf"), Distance, StartChaseDistance));
		Move(DeltaTime, Direction);
		CanMoreChase = true;

		// もうすぐ追わなくなる警告
		if (Distance >= GiveUpDistance * 0.8)
		{
			if (ChirpSE)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ChirpSE, GetActorLocation());
			}
		}
	}
	else
	{
		// 距離内が3/4の確率でもうちょっと接近する
		if (CanMoreChase && FMath::SRand() < 0.8f)
		{
			Move(DeltaTime, Direction);
		}
		else
		{
			CanMoreChase = false;
		}
	}
}

void AAnimal::Move(float DeltaTime, FVector Direction)
{
	// 向き変更
	FRotator TargetRotation = Direction.Rotation();
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.f));

	// 移動
	GetCharacterMovement()->AddInputVector(Direction.GetSafeNormal() * Speed * DeltaTime);
}

bool AAnimal::GiveUp()
{
	ACharacter* Player = CurrentTarget.Get();

	FVector PlayerLocation = Player->GetActorLocation();
	FRotator PlayerRotation = Player->GetActorRotation();
	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = PlayerLocation + PlayerRotation.RotateVector(RelativeOffset);
	float TotalDistance = FVector::DistXY(MyLocation, TargetLocation);

	// 一定以上距離を離れたら追わなくなる
	if (TotalDistance >= GiveUpDistance)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("%s distance: %lf"), *GetFName().ToString(), TotalDistance));
		AnimalManager->ReduceAnimal(this);
		CurrentTarget = nullptr;
		return true;
	}

	return false;
}

void AAnimal::CountDownChangeOffset(float DeltaTime)
{
	TimeCnt -= DeltaTime;

	// 新しい偏移量を決める
	if (TimeCnt <= 0.f)
	{
		// 新しい偏移を設置
		ChangeOffset(GetNewOffset());
		// 次の変更時間を更新
		TimeCnt = GetNextChangeOffsetTime();
	}
}

float AAnimal::GetNextChangeOffsetTime()
{
	return ChangeOffsetTime + FMath::RandRange(-RandomDeviation, RandomDeviation);
}

FVector AAnimal::GetNewOffset()
{
	// 今プレイヤーの正面は0度になっている
	// ランダムな角度(30度 ~ 330度)
	float Angle = FMath::FRandRange((float)1 / 4 * PI, (float)7 / 4 * PI);

	// ランダムな距離（0～追い始まる距離の８割）、均等分布のため sqrt を使う
	float Distance = FMath::Sqrt(FMath::RandRange(0.0f, 1.0f)) * StartChaseDistance * 0.8;

	return FVector(FMath::Cos(Angle) * Distance, FMath::Sin(Angle) * Distance, 0);
}
