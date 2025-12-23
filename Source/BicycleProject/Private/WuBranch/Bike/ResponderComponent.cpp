// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/ResponderComponent.h"
#include "WuBranch/UI/QuestionUIActor.h"
#include "UntakuBranch/Question.h"
#include <Kismet/GameplayStatics.h>
#include <WuBranch/QuestionGameMode.h>
#include <WuBranch/Bike/BikeCharacter.h>
#include <WuBranch/MyGameInstance.h>
#include <WuBranch/Device/DeviceManager.h>
#include "WuBranch/Bike/BikeComponent.h"

// Sets default values for this component's properties
UResponderComponent::UResponderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UResponderComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		BikeCom = Character->GetComponentByClass<UBikeComponent>();
	}
}

// Called every frame
//void UResponderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UResponderComponent::OnSelectLeftAnswer()
{
	FQuestion* question = QuestionActor->GetNowQuestion();
	SelectLeftAnswer(question->ID, 0);
}

void UResponderComponent::SelectLeftAnswer(int questionID, int answer)
{
	HandleSelectAnswer(FRotator(0.0f, -90.0f, 0.0f));
	//出口まで誘導
	QuestionActor->UseLeftExit();
	// 2025.10.19 ウー start クイズをなくしたい要望に応じての修正
	//答え合わせ
	AQuestionGameMode* GameMode = Cast<AQuestionGameMode>(UGameplayStatics::GetGameMode(this));
	//bool Result = GameMode->CheckAnswer(questionID, answer);
	GameMode->AnsweredQuestion();
	// 正解か不正解を表示
	//QuestionActor->SetResult(0, Result);
	// コインの処理
	ABikeCharacter* Character = Cast<ABikeCharacter>(GetOwner());
	//HandleCoin(Result, !Character->HasOverSpeed());
	BikeCom->HandleCoin(true, !Character->HasOverSpeed());
	// 2025.10.19 ウー end
	// 超速の記録をリセット
	Character->ResetOverSpeed();
	// マップの生成
	BikeCom->SpawnNextMap(true);
}

void UResponderComponent::OnSelectRightAnswer()
{
	FQuestion* question = QuestionActor->GetNowQuestion();
	SelectRightAnswer(question->ID, 1);
}

void UResponderComponent::SelectRightAnswer(int questionID, int answer)
{
	HandleSelectAnswer(FRotator(0.0f, 90.0f, 0.0f));
	//出口まで誘導
	QuestionActor->UseRightExit();
	// 2025.10.19 ウー start クイズをなくしたい要望に応じての修正
	//答え合わせ
	AQuestionGameMode* GameMode = Cast<AQuestionGameMode>(UGameplayStatics::GetGameMode(this));
	//bool Result = GameMode->CheckAnswer(questionID, answer);
	GameMode->AnsweredQuestion();
	// 正解か不正解を表示
	//QuestionActor->SetResult(1, Result);
	// コインの処理
	ABikeCharacter* Character = Cast<ABikeCharacter>(GetOwner());
	//HandleCoin(Result, !Character->HasOverSpeed());
	BikeCom->HandleCoin(true, !Character->HasOverSpeed());
	// 超速の記録をリセット
	Character->ResetOverSpeed();
	// マップの生成
	BikeCom->SpawnNextMap(false);
}

void UResponderComponent::HandleSelectAnswer(FRotator dir)
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

void UResponderComponent::DisableSelectAnswerAction()
{
	UMyGameInstance* GameInstance = GetOwner()->GetGameInstance<UMyGameInstance>();
	UDeviceManager* DeviceManager = GameInstance->GetDeviceManager();
	DeviceManager->DisableSelectAnswerActions();
}