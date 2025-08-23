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
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	_speed = 50.0f;
	_inertiaDamping = 10.0f;
	_inertiaVelocity = FVector::ZeroVector;
}


// Called when the game starts
void UBikeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	_isAutoPlay = false;
	CoinsOfQuiz = 0;
}

// Called every frame
void UBikeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (_isAutoPlay)
	{
		FVector deltaPos = FMath::VInterpTo(GetOwner()->GetActorLocation(), _synchronizePos, DeltaTime, 2);
		GetOwner()->SetActorLocation(deltaPos);
	}
	else
	{
		HandleInertia(DeltaTime);
	}

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
	_isAutoPlay = true;
	_questionActor = actor;
}

void UBikeComponent::DisableAutoPlay()
{
	_isAutoPlay = false;
	_questionActor = nullptr;
}

bool UBikeComponent::GetIsAutoPlay() const
{
	return _isAutoPlay;
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
	//UKismetSystemLibrary::PrintString(this, "Recieve Move input: " + direction.ToString(), true, false, FColor::Green, 10.f);

	// 移動方向は自転車今向いている方向を中心に
	FVector actorForward = GetOwner()->GetActorForwardVector();
	FVector actorRight = GetOwner()->GetActorRightVector();
	FVector dir = FVector::ZeroVector;
	// バックさせない
	FVector2D bikeDir = direction;
	if (bikeDir.X < 0)
		bikeDir.X = 0;
	dir = actorForward * bikeDir.X + actorRight * bikeDir.Y;

	// 移動
	// AddForceで移動すると、VRの中で小さい揺れが発生して酔いやすくなるので破棄してACharacterのCharacterMovementを利用します
	ABikeCharacter* character = Cast<ABikeCharacter>(GetOwner());
	character->AddMovementInput(actorForward, bikeDir.X);
	character->AddMovementInput(actorRight, bikeDir.Y);

	// 慣性を設定
	_inertiaVelocity = dir.GetSafeNormal() * _speed;
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
	//答え合わせ
	AQuestionGameMode* GameMode = Cast<AQuestionGameMode>(UGameplayStatics::GetGameMode(this));
	bool Result = GameMode->CheckAnswer(questionID, answer);
	// コインの処理
	ABikeCharacter* Character = Cast<ABikeCharacter>(GetOwner());
	HandleCoin(Result, !Character->HasOverSpeed());
	// マップの生成
	SpawnMap(true);
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
	//答え合わせ
	AQuestionGameMode* GameMode = Cast<AQuestionGameMode>(UGameplayStatics::GetGameMode(this));
	bool Result = GameMode->CheckAnswer(questionID, answer);
	// コインの処理
	ABikeCharacter* Character = Cast<ABikeCharacter>(GetOwner());
	HandleCoin(Result, !Character->HasOverSpeed());
	// マップの生成
	SpawnMap(false);
}

void UBikeComponent::DisableSelectAnswerAction()
{
	UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetOwner()->GetWorld()->GetGameInstance());
	UDeviceManager* deviceManager = gameInstance->GetDeviceManager();
	deviceManager->DisableSelectAnswerActions();
}

void UBikeComponent::SpawnMap(bool IsLeft)
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
		GameInstance->AddCoins(CoinsOfQuiz);
	// リセット
	CoinsOfQuiz = 0;
}
