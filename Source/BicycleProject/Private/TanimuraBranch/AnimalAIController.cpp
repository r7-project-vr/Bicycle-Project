// Fill out your copyright notice in the Description page of Project Settings.

#include "TanimuraBranch/AnimalAIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

AAnimalAIController::AAnimalAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	bHasTarget = false;
	StuckCheckTimer = 0.0f;
	bIsWaiting = false;
	WaitTimer = 0.0f;

	// 地面情報の初期化
	CurrentGroundNormal = FVector::UpVector;
	bHasGroundNormal = false;
}

void AAnimalAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ACharacter* PossessedCharacter = Cast<ACharacter>(InPawn))
	{
		// コントローラーYawは使わず、独自で回転させる
		PossessedCharacter->bUseControllerRotationYaw = false;

		// 移動方向に自動で向かないようにする
		if (UCharacterMovementComponent* MoveComp = PossessedCharacter->GetCharacterMovement())
		{
			MoveComp->bOrientRotationToMovement = false;
		}
	}
}

void AAnimalAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetPawn())
	{
		Action(DeltaTime);
	}
}

void AAnimalAIController::DecideBehavior()
{
	ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());
	if (!ControlledCharacter) return;

	// マップ矩形内でランダム地点を決定
	const float RandomX = UKismetMathLibrary::RandomFloatInRange(-14900.0f, 1500.0f);
	const float RandomY = UKismetMathLibrary::RandomFloatInRange(-14900.0f, 14900.0f);
	FVector GoalLocation(RandomX, RandomY, ControlledCharacter->GetActorLocation().Z);

	// 下向きにレイを飛ばして地面を探す
	FVector TraceStart = GoalLocation;
	TraceStart.Z += 5000.0f;
	FVector TraceEnd = GoalLocation;
	TraceEnd.Z -= 5000.0f;

	// トレース結果
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ControlledCharacter);

	// 地形に当たる位置を目標とする
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams);

	if (bHit)
	{
		// ヒット地点を目標にする
		CurrentTargetLocation = HitResult.Location;
		bHasTarget = true;

		// スタック計測の初期化
		LastCheckLocation = ControlledCharacter->GetActorLocation();
		StuckCheckTimer = 0.0f;
	}
	else
	{
		// 地面が取れなかったら目標なし
		bHasTarget = false;
	}
}

void AAnimalAIController::Action(float DeltaTime)
{
	ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());
	if (!ControlledCharacter) return;

	// 待機中の処理
	if (bIsWaiting)
	{
		WaitTimer += DeltaTime;

		// 待機時間を超えたら次の行動
		if (WaitTimer >= WaitAfterArriveSeconds)
		{
			bIsWaiting = false;
			WaitTimer = 0.0f;
			DecideBehavior();
		}

		// 姿勢補正
		UpdateSlopeAlignment(DeltaTime, ControlledCharacter->GetActorForwardVector());
		return;
	}

	// 目標がなければ新しく決定
	if (!bHasTarget)
	{
		DecideBehavior();
		UpdateSlopeAlignment(DeltaTime, ControlledCharacter->GetActorForwardVector());
		return;
	}

	// 目標までの距離を計測
	const float Distance = FVector::Dist(ControlledCharacter->GetActorLocation(), CurrentTargetLocation);

	// 目標到達判定
	if (Distance <= AcceptanceRadius)
	{
		// 目標に到着
		bHasTarget = false;

		// 移動入力の残りを消す
		ControlledCharacter->ConsumeMovementInputVector();

		// 待機に入る
		bIsWaiting = true;
		WaitTimer = 0.0f;

		UpdateSlopeAlignment(DeltaTime, ControlledCharacter->GetActorForwardVector());
		return;
	}

	// 目標方向ベクトル
	const FVector Direction = (CurrentTargetLocation - ControlledCharacter->GetActorLocation()).GetSafeNormal();

	// 勾配による速度補正
	const float SlopeSpeedScale = ComputeSlopeSpeedScale(Direction);

	// 移動入力を与える
	ControlledCharacter->AddMovementInput(Direction, MoveSpeedScale * SlopeSpeedScale);

	StuckCheckTimer += DeltaTime;
	if (StuckCheckTimer >= StuckCheckInterval)
	{
		StuckCheckTimer = 0.0f;

		// 一定時間の移動量を計測
		const float MovedDist = FVector::Dist(ControlledCharacter->GetActorLocation(), LastCheckLocation);

		// 移動距離が閾値より小さければスタック判定
		if (MovedDist < StuckThresholdDistance)
		{
			bHasTarget = false;
			DecideBehavior();
		}

		// 次のチェック用に更新
		LastCheckLocation = ControlledCharacter->GetActorLocation();
	}

	// 姿勢補正
	UpdateSlopeAlignment(DeltaTime, Direction);
}

void AAnimalAIController::UpdateSlopeAlignment(float DeltaTime, const FVector& MoveDirection)
{
	ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());
	if (!ControlledCharacter) return;

	// キャラの上下にレイを飛ばして地面を取得
	const FVector PawnLocation = ControlledCharacter->GetActorLocation();
	const FVector TraceStart = PawnLocation + FVector(0.0f, 0.0f, GroundTraceUpOffset);
	const FVector TraceEnd = PawnLocation - FVector(0.0f, 0.0f, GroundTraceDownOffset);

	// トレース結果
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ControlledCharacter);

	// 地面が取れたら姿勢補正を行う
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
	{
		// 地面法線を保存
		CurrentGroundNormal = HitResult.Normal;
		bHasGroundNormal = true;

		// 勾配角度を産出
		const float SlopeAngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CurrentGroundNormal, FVector::UpVector)));

		// 移動方向は水平に投影してYaw取得
		const FVector FlatMoveDir = FVector(MoveDirection.X, MoveDirection.Y, 0.0f).GetSafeNormal();
		const bool bHasMoveDir = !FlatMoveDir.IsNearlyZero();

		// 平地ならYawのみ更新
		if (SlopeAngleDeg < SlopeAlignMinAngleDeg)
		{
			const FRotator CurrentRotation = ControlledCharacter->GetActorRotation();
			const float TargetYaw = bHasMoveDir ? FlatMoveDir.Rotation().Yaw : CurrentRotation.Yaw;
			const FRotator TargetRotation(0.0f, TargetYaw, 0.0f);

			// 滑らかに回転
			const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, SlopeAlignInterpSpeed);
			ControlledCharacter->SetActorRotation(NewRotation);
			return;
		}

		// 勾配時は地面平面に投影したForwardを使う
		FVector ProjectedForward = FVector::VectorPlaneProject(MoveDirection, CurrentGroundNormal).GetSafeNormal();
		if (ProjectedForward.IsNearlyZero())
		{
			ProjectedForward = ControlledCharacter->GetActorForwardVector();
		}

		// 地面法線に沿った回転を作成
		const FRotator TargetRotation = FRotationMatrix::MakeFromXZ(ProjectedForward, CurrentGroundNormal).Rotator();
		const FRotator NewRotation = FMath::RInterpTo(ControlledCharacter->GetActorRotation(), TargetRotation, DeltaTime, SlopeAlignInterpSpeed);

		// 回転を適用
		ControlledCharacter->SetActorRotation(NewRotation);
	}
	else
	{
		// 地面が取れない場合は法線無効
		bHasGroundNormal = false;
	}
}

float AAnimalAIController::ComputeSlopeSpeedScale(const FVector& MoveDirection) const
{
	// 地面法線が無効なら減速しない
	if (!bHasGroundNormal) return 1.0f;

	// 勾配角度を算出
	const float SlopeAngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CurrentGroundNormal, FVector::UpVector)));
	if (SlopeAngleDeg <= KINDA_SMALL_NUMBER) return 1.0f;

	// 下り方向ベクトルを計算
	const FVector DownSlopeDir = FVector::VectorPlaneProject(-FVector::UpVector, CurrentGroundNormal).GetSafeNormal();
	if (DownSlopeDir.IsNearlyZero()) return 1.0f;

	// 上り方向ベクトル
	const FVector UpSlopeDir = -DownSlopeDir;

	// 移動方向を正規化
	const FVector MoveDir = MoveDirection.GetSafeNormal();

	// 上り坂のときだけ速度を落とす
	const float UphillDot = FVector::DotProduct(MoveDir, UpSlopeDir);
	if (UphillDot <= 0.1f) return 1.0f;

	// 角度に応じて減速率を算出
	const float Alpha = FMath::Clamp(SlopeAngleDeg / UphillMaxAngleDeg, 0.0f, 1.0f);
	return FMath::Lerp(1.0f, UphillMinSpeedScale, Alpha);
}

