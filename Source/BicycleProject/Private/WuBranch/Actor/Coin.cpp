// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Coin.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "WuBranch/Bike/BikeCharacter.h"
#include "WuBranch/Bike/BikeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/Actor/Component/CoinSpawnerComponent.h"

// Sets default values
ACoin::ACoin()
	: Maker(nullptr)
	, IsRotating(false)
	, MoveCnt(0.f)
	, RotateTimeCnt(0.f)
	, MoveDistanceForAnimation(0.f)
	, IsPause(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RootComponent = Collision;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ACoin::BeginPlay()
{
	Super::BeginPlay();
	
	MoveCnt = 0.f;
	IsRotating = false;
	IsPause = false;
	OnActorBeginOverlap.AddDynamic(this, &ACoin::OnOverlapBegin);
	//BaseLocation = GetActorLocation();
	//BaseRotation = GetActorRotation();
	BaseLocation = Mesh->GetRelativeLocation();
	BaseRotation = Mesh->GetRelativeRotation();
}

// Called every frame
void ACoin::Tick(float DeltaTime)
{
	if (IsPause)
		return;

	Super::Tick(DeltaTime);

	PlayAnimation(DeltaTime);
}

void ACoin::Init(UCoinSpawnerComponent* Spawner)
{
	Maker = Spawner;
}

void ACoin::Pause_Implementation()
{
	IsPause = true;
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACoin::ReStart_Implementation()
{
	IsPause = false;
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

bool ACoin::IsPause_Implementation()
{
	return IsPause;
}

UBoxComponent* ACoin::GetCollision() const
{
	return Collision;
}

void ACoin::ChangeBaseHeight(float NewHeight)
{
	// 基本の位置を更新
	BaseLocation.Z = NewHeight;
}

void ACoin::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor)
	{
		if (OtherActor->IsA(ABikeCharacter::StaticClass()))
		{
			AddCoin(OtherActor);
			
			// エフェクト

			// SE
			if (PickupSound)
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());

			if(Maker)
				Maker->DestroyCoin(this);
			Destroy();
		}
	}
}

void ACoin::PlayAnimation(float DeltaTime)
{
	// 上下移動
	PlayMoveAnimation(DeltaTime);

	// ランダムで回転
	PlayRotateAnimation(DeltaTime);
}

void ACoin::PlayMoveAnimation(float DeltaTime)
{
	MoveCnt += DeltaTime;
	FVector DeltaMove = FVector(0.f, 0.f, MoveDistanceForAnimation * FMath::Sin(MoveCnt));
	//SetActorLocation(BaseLocation + DeltaMove);
	Mesh->SetRelativeLocation(BaseLocation + DeltaMove);
}

void ACoin::PlayRotateAnimation(float DeltaTime)
{
	if (IsRotating)
	{
		RotateTimeCnt += DeltaTime;
		float Per = FMath::Clamp(RotateTimeCnt / RotationTime, 0.f, 1.f);
		int Angle = FMath::Lerp(0, 360, Per);
		Mesh->SetRelativeRotation(BaseRotation + FRotator(0, Angle, 0));
		//SetActorRotation(BaseRotation + FRotator(0, Angle, 0));
		if (Angle >= 360)
		{
			RotateTimeCnt = FMath::RandRange(RotationDelay.X, RotationDelay.Y);
			IsRotating = false;
		}
	}
	else
	{
		RotateTimeCnt -= DeltaTime;
		if (RotateTimeCnt <= 0.f)
		{
			RotateTimeCnt = 0;
			IsRotating = true;
		}
	}
}

void ACoin::AddCoin(AActor* Actor)
{
	ABikeCharacter* Character = Cast<ABikeCharacter>(Actor);
	if (UBikeComponent* BikeComp = Character->GetComponentByClass<UBikeComponent>())
	{
		BikeComp->AddCoins(1);
	}
}
