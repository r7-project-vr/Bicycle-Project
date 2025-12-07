// Fill out your copyright notice in the Description page of Project Settings.


#include "tokuamaru/CPP_GameTimer.h"

#include "WuBranch/MyGameInstance.h"
#include "WuBranch/QuestionGameMode.h"



UCPP_GameTimer::UCPP_GameTimer(const FObjectInitializer& ObjectInitializer)
	:
	Super(ObjectInitializer),
	TotalGameTime(0.0f),
	CurrentGameTime(0.0f),
	StandardStageTime(5.0f),
	LastPlayedSecond(0)
{
}

void UCPP_GameTimer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UWorld* World = GetWorld())
	{
		if (UMyGameInstance* GI = Cast<UMyGameInstance>(World->GetGameInstance()))
		{
			TotalGameTime = GI->GetNumOfSets();
			CurrentGameTime = TotalGameTime * 60.0f * StandardStageTime;
		}
		else {

			UE_LOG(LogTemp, Error, TEXT("Could not got MyGameInstance GameTimer"));
			TotalGameTime = 1.0f;
			CurrentGameTime = TotalGameTime * 60.0f * StandardStageTime;
		}
	}
	else {

		UE_LOG(LogTemp, Error, TEXT("Could not got world GameTimer"));
	}

	if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
	{
		if (AQuestionGameMode* QGM = Cast<AQuestionGameMode>(GM))
		{
			QuestionGameMode = QGM;

		}
		else {

			UE_LOG(LogTemp, Error, TEXT("Could not got GameMode GameTimer"));
		}
	}
	else {

		UE_LOG(LogTemp, Error, TEXT("Could not got GameMode GameTimer"));
	}

}

void UCPP_GameTimer::NativeConstruct()
{
	Super::NativeConstruct();

	if (TimerText)
	{
		TimerText->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
		SetText();

	}

}

void UCPP_GameTimer::DecreaseGameTime(float delta)
{
	if (CurrentGameTime > 0.0f) {
		CurrentGameTime -= 1.0f * delta;

		SoundUpDate();

		if (CurrentGameTime <= 60.0f && TimerText)
		{
			TimerText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}

		SetText();
	}
	else {
		if (QuestionGameMode) {
			if (IsGameScene) {
				QuestionGameMode->FinishGame();
				IsGameScene = false;
			}
		}
	}

}

void UCPP_GameTimer::SetText()
{
	if (!TimerText)return;
	int32 Minutes = (int32)(CurrentGameTime / 60.0f);
	int32 Seconds = (int32)(fmod(CurrentGameTime, 60.0f));
	if (Minutes >= 10 && Seconds >= 10) {
		TimerText->SetText(FText::FromString(FString::Printf(TEXT("%d：%d"), Minutes, Seconds)));
	}
	else if (Minutes < 10 && Seconds < 10) {
		TimerText->SetText(FText::FromString(FString::Printf(TEXT("0%d：0%d"), Minutes, Seconds)));
	}
	else if (Minutes < 10) {
		TimerText->SetText(FText::FromString(FString::Printf(TEXT("0%d：%d"), Minutes, Seconds)));
	}
	else if (Seconds < 10) {
		TimerText->SetText(FText::FromString(FString::Printf(TEXT("%d：0%d"), Minutes, Seconds)));
	}
}

void UCPP_GameTimer::SoundUpDate()
{
	if (!SignalSound)return;
	int32 PlayTime = FMath::FloorToInt((TotalGameTime * 60.0f) - CurrentGameTime);

	if (PlayTime % (int32)SoundInterval == 0 && PlayTime != LastPlayedSecond)
	{
		UGameplayStatics::PlaySound2D(this, SignalSound);
		LastPlayedSecond = PlayTime;
	}
}


