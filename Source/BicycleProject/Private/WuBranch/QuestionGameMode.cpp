// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/QuestionGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "WuBranch/Bike/BikeCharacter.h"
#include "WuBranch/Bike/BikeComponent.h"
#include "WuBranch/BikePlayerController.h"
#include "WuBranch/Actor/Firework.h"
#include "WuBranch/UI/QuestionUIActor.h"
#include <WuBranch/MyGameInstance.h>
#include "WuBranch/Device/DeviceManager.h"
#include <UntakuBranch/Question.h>
#include "WuBranch/Actor/EndPosition.h"
#include "UntakuBranch/QuestionManager.h"
#include <ShiiBranch/OptionUIWidget.h>
#include <Components/WidgetComponent.h>
#include "WuBranch/MyGameInstance.h"

void AQuestionGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	_player = Cast<ABikeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	// set bike mesh
	//_player->SetBikeMesh();

	_playerController = Cast<ABikePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	QuestionManager = Cast<AQuestionManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AQuestionManager::StaticClass()));

	CorrectNum = 0;
	WrongNum = 0;
	QuestionIndex = 0;
	CurrentState = QuestionGameState::Playing;
	// 2025.11.19 谷村 start
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Get Game Instance Error!"));
	}
	else
	{
		SuccessCondition = GameInstance->NumOfSets;
	}
	// 2025.11.19 谷村 end

	GetAllQuestions();
}

void AQuestionGameMode::PassTheGoal(AActor* passedActor)
{
	if (_player && passedActor == _player)
	{
		// 自転車への制御を強制的にオフにする
		UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
		GameInstance->GetDeviceManager()->DisableDefaultActions();
		GameInstance->GetDeviceManager()->DisConnectAllDevices();
		/*if (_playerController)
		{
			_playerController->SetPlayerEnabledState(false);
		}*/
		_player->GetBikeComponent()->ReduceVelocityTo0();

		if (CurrentState == QuestionGameState::Playing)
			return;

		// ゴールに到達したらゲームクリア
		//GameOver(true);

		//5秒後に次の世界に行く
		// 注意!!レベル名は間違わないように!!
		FTimerDelegate timerDelegate;
		if(CurrentState == QuestionGameState::Successed)
			timerDelegate.BindUFunction(this, "ChangeLevel", true);
		else if (CurrentState == QuestionGameState::Failed)
			timerDelegate.BindUFunction(this, "ChangeLevel", false);
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, timerDelegate, 5.f, false);
	}
}

bool AQuestionGameMode::CheckAnswer(int32 questionID, int32 answer)
{
	// 問題システムに問題IDと解答を送って答えをもらう
	bool Result = QuestionManager->CheckPlayerAnswerInLastRandom(questionID, answer);
	
	if (Result)
	{
		// 正解の数を計算
		CorrectNum++;
		// SE
		UGameplayStatics::PlaySound2D(GetWorld(), CorrectSound);
	}
	else
	{
		// 不正解の数を計算
		WrongNum++;
		// SE
		UGameplayStatics::PlaySound2D(GetWorld(), WrongSound);
	}
	
	// 答えを保存
	// 問題IDがquestionIDの問題を見つける
	FQuestion* Question = *_questions.FindByPredicate([questionID](const FQuestion* question) {
		return question->ID == questionID;
	});
	if (Question)
	{
		Question->PlayerAnswer = answer;
	}

	// UI更新
	UpdateAnswerUI();

	// ゲームオーバー
	if (IsGameFailed())
	{
		EndGame(false);
	}
	// ゲームクリア
	else if (IsGameClear())
	{
		EndGame(true);
	}
	return Result;
}

void AQuestionGameMode::AnsweredQuestion()
{
	CorrectNum++;
	// UI更新
	UpdateAnswerUI();

	// ゲームオーバー
	if (IsGameFailed())
	{
		EndGame(false);
	}
	// ゲームクリア
	else if (IsGameClear())
	{
		EndGame(true);
	}
}

int AQuestionGameMode::GetCurrectNumber() const
{
	return CorrectNum;
}

int AQuestionGameMode::GetWrongNumber() const
{
	return WrongNum;
}

void AQuestionGameMode::GetAllQuestions()
{
	// 問題管理者から問題をゲット
	if (!QuestionManager)
	{
		UE_LOG(LogTemp, Error, TEXT("question Manager is not found!!"));
		return;
	}

	// 問題の総数はゲームオーバーになる不正解数とゲームクリアになる正解数の合計
	_questions = QuestionManager->GetQuizs(FailCondition + SuccessCondition);
	if (_questions.Num() <= 0)
		UE_LOG(LogTemp, Error, TEXT("Get questions failed!!"));
}

FQuestion* AQuestionGameMode::GetQuestion()
{
	if (_questions.Num() > 0)
	{
		return _questions[QuestionIndex++];
	}
	return nullptr;
}

bool AQuestionGameMode::IsAnswered(int32 QuestionID)
{
	FQuestion* Question = *_questions.FindByPredicate([QuestionID](const FQuestion* question) {
		return question->ID == QuestionID;
		});
	if (Question)
	{
		return Question->PlayerAnswer != -1;
	}
	return false;
}

bool AQuestionGameMode::IsGameFailed() const
{
	if(FailCondition <= 0)
		return false;
	return WrongNum >= FailCondition;
}

bool AQuestionGameMode::IsGameClear() const
{
	if(SuccessCondition <= 0)
		return false;
	return CorrectNum >= SuccessCondition;
}

int AQuestionGameMode::GetSuccessCondition() const
{
	return SuccessCondition;
}

void AQuestionGameMode::SetSuccessCondition(int32 Num)
{
	SuccessCondition = Num;
}

void AQuestionGameMode::UpdateAnswerUI()
{
	OnUpdateAnswerUIDelegate.Broadcast(CorrectNum, WrongNum);
}

void AQuestionGameMode::EndGame(bool GameResult)
{
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (GameResult)
	{
		CurrentState = QuestionGameState::Successed;
		UKismetSystemLibrary::PrintString(this, "GameClear!", true, false, FColor::Green, 10.f);
	}
	else
	{
		CurrentState = QuestionGameState::Failed;
		UKismetSystemLibrary::PrintString(this, "GameOver!", true, false, FColor::Red, 10.f);
	}
	// すべての問題を無効にする
	DisableAllQuestions();
	// クイズを記録
	//GameInstance->SaveQuizsForResult(_questions);
	// 結果を記録
	GameInstance->SetGameResult(GameResult);
	// ゴールをプレイヤーの進行先に置く
	PlaceGoal();
}

void AQuestionGameMode::HandleGameSuccess(int32 questionID)
{
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	CurrentState = QuestionGameState::Successed;
	// ゲームクリア
	UKismetSystemLibrary::PrintString(this, "GameClear!", true, false, FColor::Green, 10.f);
	// すべての問題を無効にする
	DisableAllQuestions();
	// クイズを記録
	GameInstance->SaveQuizsForResult(_questions);
	// 結果を記録
	GameInstance->SetGameResult(true);
	// ゴールをプレイヤーの進行先に置く
	PlaceGoal(questionID);
}

void AQuestionGameMode::HandleGameFailed(int32 questionID)
{
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	CurrentState = QuestionGameState::Failed;
	// UIの表示
	UKismetSystemLibrary::PrintString(this, "GameOver!", true, false, FColor::Red, 10.f);
	// すべての問題を無効にする
	DisableAllQuestions();
	// クイズを記録
	GameInstance->SaveQuizsForResult(_questions);
	// 結果を記録
	GameInstance->SetGameResult(false);
	// ゴールをプレイヤーの進行先に置く
	PlaceGoal(questionID);
}

void AQuestionGameMode::ChangeLevel(bool IsSucc)
{
	// クリア
	if (IsSucc)
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LoadSuccLevel);
	}
	// 失敗
	else
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LoadFailLevel);
	}
}

void AQuestionGameMode::DisableAllQuestions()
{
	TArray<AActor*> QuestionActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestionUIActor::StaticClass(), QuestionActors);
	for (AActor* Actor : QuestionActors)
	{
		if (AQuestionUIActor* Question = Cast<AQuestionUIActor>(Actor))
		{
			Question->DisableFeature();
		}
	}
}

void AQuestionGameMode::PlaceGoal(int32 QuestionID)
{
	AActor* Goal = FindGoal();
	if (!Goal)
		return;

	// 問題を特定
	AQuestionUIActor* Target = QuestionID == -1 ? FindQuestion() : FindQuestion(QuestionID);
	if (!Target)
		return;

	// 問題の出口の位置と向きをゲット
	FVector startLocation, forward;
	if (Target->GetExitLocationAndForward(startLocation, forward))
	{
		// ゴールを進行先に置く
		float distance = 5000.0f;
		startLocation.Z = 100.f;
		Goal->SetActorLocation(startLocation + forward * distance);
		Goal->SetActorRotation((forward * -1).Rotation());
	}
}

AActor* AQuestionGameMode::FindGoal()
{
	// 地図上のゴールを探す
	TArray<AActor*> Goals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEndPosition::StaticClass(), Goals);
	if (Goals.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Did not find Goal!!"));
		return nullptr;
	}
	return Goals[0];
}

AQuestionUIActor* AQuestionGameMode::FindQuestion(int32 QuestionID)
{
	TArray<AActor*> QuestionActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestionUIActor::StaticClass(), QuestionActors);
	for (AActor* Actor : QuestionActors)
	{
		if (AQuestionUIActor* Question = Cast<AQuestionUIActor>(Actor))
		{
			if (UOptionUIWidget* QuestionUI = Cast<UOptionUIWidget>(Question->GetWidgetComponent()->GetWidget()))
			{
				// UIからもらったquestionIDと比べて目標となっているアクターをゲット
				if (QuestionUI->GetQuestionID() == QuestionID)
				{
					return Question;
				}
			}
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Did not find Question Actor!!"));
	return nullptr;
}

AQuestionUIActor* AQuestionGameMode::FindQuestion()
{
	TArray<AActor*> QuestionActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestionUIActor::StaticClass(), QuestionActors);
	for (AActor* Actor : QuestionActors)
	{
		if (AQuestionUIActor* Question = Cast<AQuestionUIActor>(Actor))
		{
			FVector QuestionLoc = Actor->GetActorLocation();
			FVector PlayerLoc = _player->GetActorLocation();
			double Distance = FVector::Dist2D(QuestionLoc, PlayerLoc);
			if (Distance <= 1500.f)
				return Question;
		}
	}
	return nullptr;
}
