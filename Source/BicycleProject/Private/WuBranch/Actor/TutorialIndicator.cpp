// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/TutorialIndicator.h"
#include "GameFramework/Character.h"

// Sets default values
ATutorialIndicator::ATutorialIndicator()
	: TargetLocation(FVector::ZeroVector)
	, RotationSpeed(1.f)
	, DistanceFromTarget(15.f)
	, MoveAnimationCounter(0.f)
	, MoveDistanceForAnimation(5.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCastShadow(false);

	this->SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void ATutorialIndicator::BeginPlay()
{
	Super::BeginPlay();
	
	Player = GetWorld()->GetFirstPlayerController()->GetCharacter();
	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("Didnot find Player"));
	}
	SetActorHiddenInGame(true);
}

// Called every frame
void ATutorialIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!IsHidden())
		MoveAnimation(DeltaTime);
}

void ATutorialIndicator::SetTarget(FVector Target)
{
	TargetLocation = Target;
	
	DecideSelfLocation();
	FaceToTarget();
}

void ATutorialIndicator::ShowArrow()
{
	this->SetActorHiddenInGame(false);
	// 表示するアニメーションやエフェクトを追加する場合はここに記述
}

void ATutorialIndicator::HideArrow()
{
	// 非表示にするアニメーションやエフェクトを追加する場合はここに記述
	this->SetActorHiddenInGame(true);
}

void ATutorialIndicator::DecideSelfLocation()
{
	if (!Player)
		return;

	//目標やプレイヤーの位置から自身の位置を決定する
	FVector ToTarget = TargetLocation - Player->GetActorLocation();
	FVector PlayerForward = Player->GetActorForwardVector();
	PlayerForward.Z = 0.f;
	ToTarget.Z = 0.f;
	float CrossZ = FVector::CrossProduct(PlayerForward, ToTarget).Z;

	// プレイヤーに向いて目標の隣に配置する
	FVector SideDirection = FVector::CrossProduct(FVector::UpVector, ToTarget).GetSafeNormal();
	// 目標は左側にいる場合は自身を目標の右側に配置する
	// 右側にいる場合は自身を目標の左側に配置する
	float Dir = CrossZ >= 0.f ? 1.f : -1.f;
	FVector Location = TargetLocation + SideDirection * Dir * DistanceFromTarget;
	SetActorLocation(Location);
}

void ATutorialIndicator::FaceToTarget()
{
	FVector Direction = TargetLocation - GetActorLocation();
	SetActorRotation(Direction.Rotation());
}

void ATutorialIndicator::MoveAnimation(float DeltaTime)
{
	MoveAnimationCounter += DeltaTime;
	FVector DeltaMove = FVector(0.f, 0.f, MoveDistanceForAnimation * FMath::Sin(MoveAnimationCounter));
	MeshComponent->SetRelativeLocation(GetActorLocation() + DeltaMove);
}

