// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Coin.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "WuBranch/Bike/BikeCharacter.h"
#include "WuBranch/MyGameInstance.h"

// Sets default values
ACoin::ACoin()
	: MoveCnt(0.f)
	, IsRotating(false)
	, RotateTimeCnt(0.f)
	, MoveDistanceForAnimation(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
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
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACoin::OnOverlapBegin);
	BaseLocation = GetActorLocation();
	BaseRotation = GetActorRotation();
}

// Called every frame
void ACoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayAnimation(DeltaTime);
}

void ACoin::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (OtherActor->IsA(ABikeCharacter::StaticClass()))
		{
			AddCoin();
			
			// エフェクト

			// SE
			if(PickupSound)
				

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
	SetActorLocation(BaseLocation + DeltaMove);
}

void ACoin::PlayRotateAnimation(float DeltaTime)
{
	if (IsRotating)
	{
		RotateTimeCnt += DeltaTime;
		float per = FMath::Clamp(RotateTimeCnt / RotationTime, 0.f, 1.f);
		int Angle = FMath::Lerp(0, 360, per);
		SetActorRotation(BaseRotation + FRotator(0, Angle, 0));
		if (Angle >= 360)
		{
			RotateTimeCnt = FMath::RandRange(RotationDelay.X, RotationDelay.Y);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Time: %lf"), RotateTimeCnt));
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

void ACoin::AddCoin()
{
	UMyGameInstance* GI = GetGameInstance<UMyGameInstance>();
	GI->AddCoins(1);
}
