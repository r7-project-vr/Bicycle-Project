// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/BikeMovementComponent.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "GameFramework/Character.h"
#include "WuBranch/Bike/BikeComponent.h"
#include <WuBranch/Bike/BikeCharacter.h>

// Sets default values for this component's properties
UBikeMovementComponent::UBikeMovementComponent()
	: Owner(nullptr)
	, BikeCom(nullptr)
	, InertiaSpeed(50.0f)
	, InertiaDamping(10.0f)
	, InertiaVelocity(FVector::ZeroVector)
	, bHasMovInput(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBikeMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	bIsAutoPlay = false;
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		Owner = Character;
		BikeCom = Owner->GetComponentByClass<UBikeComponent>();
	}
	AutoAccelerationTimer = 0.0f;
}


// Called every frame
void UBikeMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	// オートプレイ中なら
	if (bIsAutoPlay)
	{
		if ((SynchronizePos - GetOwner()->GetActorLocation()).SizeSquared2D() <= FMath::Square(10.f))
		{
			// 目標地点についたら通知
			OnArrivedLocationEvent.Broadcast();
		}
		else
		{
			// 目標地点に移動
			FVector DeltaPos = FMath::VInterpTo(GetOwner()->GetActorLocation(), SynchronizePos, DeltaTime, 2);
			GetOwner()->SetActorLocation(DeltaPos);
		}
	}
	// タイマーが残っているなら
	else if (AutoAccelerationTimer > 0.0f)
	{
		// タイマーを減らす
		AutoAccelerationTimer -= DeltaTime;
		if (ABikeCharacter* Character = Cast<ABikeCharacter>(GetOwner()))
		{
			// CMCに移動入力を送り続ける（これが「ゆっくり加速」の実体）
			// ScaleValueを 1.0 にすれば全力加速、0.5なら緩やか
			Character->AddMovementInput(TargetMoveDirection, 0.5f);
		}
	}
	/*else if (!bHasMovInput)
	{
		HandleInertia(DeltaTime);
	}
	bHasMovInput = false;*/
}

void UBikeMovementComponent::ReduceVelocityTo0()
{
	if (Owner)
	{
		Owner->GetCharacterMovement()->StopMovementImmediately();
	}
	// 慣性の力も0にする
	InertiaVelocity = FVector::ZeroVector;
	AutoAccelerationTimer = 0.0f;
}

void UBikeMovementComponent::EnableAutoPlay()
{
	bIsAutoPlay = true;
	NotifyAutoPlay();
}

void UBikeMovementComponent::DisableAutoPlay()
{
	bIsAutoPlay = false;
	NotifyAutoPlay();
}

bool UBikeMovementComponent::GetIsAutoPlay() const
{
	return bIsAutoPlay;
}

void UBikeMovementComponent::SetSynchPos(FVector Pos)
{
	SynchronizePos = Pos;
}

void UBikeMovementComponent::HandleInertia(float DeltaTime)
{
	// 慣性で移動
	GetOwner()->AddActorWorldOffset(InertiaVelocity);

	// 減衰
	FVector Damp = InertiaVelocity.GetSafeNormal() * InertiaDamping * DeltaTime;
	if (Damp.SizeSquared() > InertiaVelocity.SizeSquared())
		InertiaVelocity = FVector::ZeroVector;
	else
		InertiaVelocity -= Damp;
}

void UBikeMovementComponent::OnMove(FVector2D Direction)
{
	if (!Owner)
		return;

	// ペナルティ中またはオートプレイ中は移動しない
	if ((BikeCom && BikeCom->IsInPenalty()) || bIsAutoPlay)
		return;

	// 加速が最大になるとペナルティ
	if (Direction.Length() == 1)
	{
		if (BikeCom)
			BikeCom->ApplyPenalty();
		return;
	}

	// 移動方向は自転車今向いている方向を中心に
	FVector ActorForward = GetOwner()->GetActorForwardVector();
	FVector ActorRight = GetOwner()->GetActorRightVector();
	FVector Dir = FVector::ZeroVector;
	// バックさせない
	FVector BikeDir = FVector(Direction.X, Direction.Y, 0.f);
	// 後退できない
	if (BikeDir.X < 0)
		BikeDir.X = 0;
	Dir = ActorForward * BikeDir.X + ActorRight * BikeDir.Y;

	// 移動
	// AddForceで移動すると、VRの中で小さい揺れが発生して酔いやすくなるので
	// 破棄してACharacterのCharacterMovementを利用します
	float MaxSpeed = Owner->GetCharacterMovement()->MaxWalkSpeed;
	// 入力した方向をキャラクターの向きに合わせる
	BikeDir = Owner->GetActorRotation().RotateVector(BikeDir);
	TargetMoveDirection = BikeDir;
	AutoAccelerationTimer = AccelerationDuration;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Tike Time: %lf"), AutoAccelerationTimer));
	//Owner->GetCharacterMovement()->Velocity = MaxSpeed * BikeDir;
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Velocity: %lf"), Owner->GetCharacterMovement()->Velocity.Length()));
	//Character->AddMovementInput(actorForward, BikeDir.X);
	//Character->AddMovementInput(actorRight, BikeDir.Y);

	// 慣性を設定
	//InertiaVelocity = dir.GetSafeNormal() * InertiaSpeed;
	bHasMovInput = true;
}

void UBikeMovementComponent::NotifyAutoPlay()
{
	OnUpdateAutoPlayEvent.Broadcast(bIsAutoPlay);
}