// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/BikeComponent.h"
#include "WuBranch/Device/DeviceManager.h"
#include <WuBranch/MyGameInstance.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <WuBranch/Bike/BikeCharacter.h>
#include "WuBranch/UI/QuestionUIActor.h"
#include <Kismet/GameplayStatics.h>
#include <WuBranch/QuestionGameMode.h>
#include "UntakuBranch/Question.h"
#include <UntakuBranch/Tile.h>

// Sets default values for this component's properties
UBikeComponent::UBikeComponent()
	: CoinsOfQuiz(0)
	, bIsPenalty(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	Speed = 50.0f;
	_inertiaDamping = 10.0f;
	_inertiaVelocity = FVector::ZeroVector;
	bHasMovInput = false;
}


// Called when the game starts
void UBikeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	bIsAutoPlay = false;
	CoinsOfQuiz = 0;
	bIsPenalty = false;
	UMyGameInstance* GameInstance = GetOwner()->GetGameInstance<UMyGameInstance>();
	if (GameInstance)
		GameInstance->ResetCoinsPerGame();
}

// Called every frame
void UBikeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (bIsAutoPlay)
	{
		if ((_synchronizePos - GetOwner()->GetActorLocation()).SizeSquared2D() <= FMath::Square(10.f))
		{
			// 目標地点についたら通知
			OnArrivedLocationEvent.Broadcast(this);
		}
		else
		{
			// 目標地点に移動
			FVector DeltaPos = FMath::VInterpTo(GetOwner()->GetActorLocation(), _synchronizePos, DeltaTime, 2);
			GetOwner()->SetActorLocation(DeltaPos);
		}
	}
	else if(!bHasMovInput)
	{
		HandleInertia(DeltaTime);
	}

	bHasMovInput = false;
	//double speed = GetOwner()->GetComponentByClass<UCharacterMovementComponent>()->Velocity.Length();
	//UKismetSystemLibrary::PrintString(this, "Speed: " + FString::SanitizeFloat(speed), true, false, FColor::Green, 10.f);
}

void UBikeComponent::ReduceVelocityTo0()
{
	GetOwner()->GetComponentByClass<UCharacterMovementComponent>()->StopMovementImmediately();
	// 慣性の力も0にする
	_inertiaVelocity = FVector::ZeroVector;
}

void UBikeComponent::EnableAutoPlay(AQuestionUIActor* actor)
{
	bIsAutoPlay = true;
	_questionActor = actor;
	NotifyAutoPlay();
}

void UBikeComponent::DisableAutoPlay()
{
	bIsAutoPlay = false;
	_questionActor = nullptr;
	NotifyAutoPlay();
}

bool UBikeComponent::GetIsAutoPlay() const
{
	return bIsAutoPlay;
}

void UBikeComponent::SetSynchPos(FVector pos)
{
	_synchronizePos = pos;
}

void UBikeComponent::HandleSelectAnswer(FRotator dir)
{
	ABikeCharacter* Character = Cast<ABikeCharacter>(GetOwner());
	// 曲がる
	Character->SetTurningAngle(dir);
	// 二回目以降選ばせない
	DisableSelectAnswerAction();
	// UI補助線を表示しない
	if (Character)
	{
		Character->DisableHintLine();
	}
}

void UBikeComponent::AddCoins(int Amount)
{
	CoinsOfQuiz += Amount;
}

void UBikeComponent::ApplyPenalty()
{
	FTimerHandle PenaltyTimerHandle;
	bIsPenalty = true;
	// ペナルティ時間後にキャンセル
	GetWorld()->GetTimerManager().SetTimer(
		PenaltyTimerHandle,
		this,
		&UBikeComponent::CancelPenalty,
		PenaltyDuration,
		false
	);
}

bool UBikeComponent::IsInPenalty() const
{
	return bIsPenalty;
}

void UBikeComponent::HandleInertia(float DeltaTime)
{
	//UKismetSystemLibrary::PrintString(this, "inertia Velocity: " + _inertiaVelocity.ToString(), true, false, FColor::Green, 10.f);
	// 慣性で移動
	GetOwner()->AddActorWorldOffset(_inertiaVelocity);

	// 減衰
	FVector damp = _inertiaVelocity.GetSafeNormal() * _inertiaDamping * DeltaTime;
	if (damp.SizeSquared() > _inertiaVelocity.SizeSquared())
		_inertiaVelocity = FVector::ZeroVector;
	else
		_inertiaVelocity -= damp;
}

void UBikeComponent::OnMove(FVector2D direction)
{
	// ペナルティ中またはオートプレイ中は移動しない
	if (bIsPenalty || bIsAutoPlay)
		return;

	// 加速が最大になるとペナルティ
	if (direction.Length() == 1)
	{
		ApplyPenalty();
		return;
	}

	// 移動方向は自転車今向いている方向を中心に
	FVector actorForward = GetOwner()->GetActorForwardVector();
	FVector actorRight = GetOwner()->GetActorRightVector();
	FVector dir = FVector::ZeroVector;
	// バックさせない
	FVector BikeDir = FVector(direction.X, direction.Y, 0.f);
	if (BikeDir.X < 0)
		BikeDir.X = 0;
	dir = actorForward * BikeDir.X + actorRight * BikeDir.Y;

	ABikeCharacter* Character = Cast<ABikeCharacter>(GetOwner());

	// 移動
	// AddForceで移動すると、VRの中で小さい揺れが発生して酔いやすくなるので
	// 破棄してACharacterのCharacterMovementを利用します
	float MaxSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
	// 入力した方向をキャラクターの向きに合わせる
	BikeDir = Character->GetActorRotation().RotateVector(BikeDir);
	Character->GetCharacterMovement()->Velocity = MaxSpeed * BikeDir;
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Velocity: %lf"), Character->GetCharacterMovement()->Velocity.Length()));	
	//Character->AddMovementInput(actorForward, BikeDir.X);
	//Character->AddMovementInput(actorRight, BikeDir.Y);

	// 慣性を設定
	_inertiaVelocity = dir.GetSafeNormal() * Speed;
	bHasMovInput = true;
}

void UBikeComponent::OnSelectLeftAnswer()
{
	FQuestion* question = _questionActor->GetNowQuestion();
	SelectLeftAnswer(question->ID, 0);
}

void UBikeComponent::SelectLeftAnswer(int questionID, int answer)
{
	HandleSelectAnswer(FRotator(0.0f, -90.0f, 0.0f));
	//出口まで誘導
	_questionActor->UseLeftExit();
	// 2025.10.19 ウー start クイズをなくしたい要望に応じての修正
	//答え合わせ
	AQuestionGameMode* GameMode = Cast<AQuestionGameMode>(UGameplayStatics::GetGameMode(this));
	//bool Result = GameMode->CheckAnswer(questionID, answer);
	GameMode->AnsweredQuestion();
	// 正解か不正解を表示
	//_questionActor->SetResult(0, Result);
	// コインの処理
	ABikeCharacter* Character = Cast<ABikeCharacter>(GetOwner());
	//HandleCoin(Result, !Character->HasOverSpeed());
	HandleCoin(true, !Character->HasOverSpeed());
	// 2025.10.19 ウー end
	// 超速の記録をリセット
	Character->ResetOverSpeed();
	// マップの生成
	SpawnNextMap(true);
}

void UBikeComponent::OnSelectRightAnswer()
{
	FQuestion* question = _questionActor->GetNowQuestion();
	SelectRightAnswer(question->ID, 1);
}

void UBikeComponent::SelectRightAnswer(int questionID, int answer)
{
	HandleSelectAnswer(FRotator(0.0f, 90.0f, 0.0f));
	//出口まで誘導
	_questionActor->UseRightExit();
	// 2025.10.19 ウー start クイズをなくしたい要望に応じての修正
	//答え合わせ
	AQuestionGameMode* GameMode = Cast<AQuestionGameMode>(UGameplayStatics::GetGameMode(this));
	//bool Result = GameMode->CheckAnswer(questionID, answer);
	GameMode->AnsweredQuestion();
	// 正解か不正解を表示
	//_questionActor->SetResult(1, Result);
	// コインの処理
	ABikeCharacter* Character = Cast<ABikeCharacter>(GetOwner());
	//HandleCoin(Result, !Character->HasOverSpeed());
	HandleCoin(true, !Character->HasOverSpeed());
	// 2025.10.19 ウー end
	// 超速の記録をリセット
	Character->ResetOverSpeed();
	// マップの生成
	SpawnNextMap(false);
}

void UBikeComponent::DisableSelectAnswerAction()
{
	UMyGameInstance* gameInstance = GetOwner()->GetGameInstance<UMyGameInstance>();
	UDeviceManager* deviceManager = gameInstance->GetDeviceManager();
	deviceManager->DisableSelectAnswerActions();
}

void UBikeComponent::SpawnNextMap(bool IsLeft)
{
	ATile* CurrentTile = FindCurrentTile();
	
	if (CurrentTile)
	{
		CurrentTile->SpawnMap(IsLeft);
	}
}

ATile* UBikeComponent::FindCurrentTile()
{
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = GetOwner()->GetActorLocation() + FVector::DownVector * 300;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FHitResult HitRes;
	bool IsHit = GetWorld()->LineTraceSingleByChannel(HitRes, Start, End, ECC_WorldDynamic, Params);
	if (IsHit)
	{
		if (ATile* Target = Cast<ATile>(HitRes.GetActor()))
		{
			return Target;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("did not find Tile"));
	return nullptr;
}

void UBikeComponent::HandleCoin(bool Result, bool NeedBonus)
{
	// クイズを正解した褒美
	if (Result)
		AddCoins(1);
	// ボーナス
	if (NeedBonus)
		AddCoins(BonusCoin);
	// 保存
	UMyGameInstance* GameInstance = GetOwner()->GetGameInstance<UMyGameInstance>();
	if (GameInstance)
		GameInstance->AddCoinsPerGame(CoinsOfQuiz);
	// リセット
	CoinsOfQuiz = 0;
}

void UBikeComponent::NotifyAutoPlay()
{
	OnUpdateAutoPlayEvent.Broadcast(bIsAutoPlay);
}

void UBikeComponent::CancelPenalty()
{
	bIsPenalty = false;
}
