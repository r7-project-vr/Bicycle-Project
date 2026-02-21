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
}

// Called when the game starts
void UBikeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

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
}

void UBikeComponent::AddCoins(int Amount)
{
	CoinsOfQuiz += Amount;
}

void UBikeComponent::ApplyPenalty()
{
	FTimerHandle PenaltyTimerHandle;
	bIsPenalty = true;
	NotifyPenaltyUpdate();
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

float UBikeComponent::GetPenaltyDuration() const
{
	return PenaltyDuration;
}

void UBikeComponent::SpawnNextMap(bool bIsLeft)
{
	ATile* CurrentTile = FindCurrentTile();
	
	if (CurrentTile)
	{
		CurrentTile->SpawnMap(bIsLeft);
	}
}

ATile* UBikeComponent::FindCurrentTile()
{
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = GetOwner()->GetActorLocation() + FVector::DownVector * 300;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FHitResult HitRes;
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitRes, Start, End, ECC_WorldDynamic, Params);
	if (bIsHit)
	{
		if (ATile* Target = Cast<ATile>(HitRes.GetActor()))
		{
			return Target;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("did not find Tile"));
	return nullptr;
}

void UBikeComponent::HandleCoin(bool bResult, bool bNeedBonus)
{
	// クイズを正解した褒美
	if (bResult)
		AddCoins(1);
	// ボーナス
	if (bNeedBonus)
		AddCoins(BonusCoin);
	// 保存
	UMyGameInstance* GameInstance = GetOwner()->GetGameInstance<UMyGameInstance>();
	if (GameInstance)
		GameInstance->AddCoinsPerGame(CoinsOfQuiz);
	// リセット
	CoinsOfQuiz = 0;
}

void UBikeComponent::CancelPenalty()
{
	bIsPenalty = false;
	NotifyPenaltyUpdate();
}

void UBikeComponent::NotifyPenaltyUpdate()
{
	if(OnUpdatePenaltyEvent.IsBound())
		OnUpdatePenaltyEvent.Broadcast(bIsPenalty);
}
