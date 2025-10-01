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

void AQuestionGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	_player = Cast<ABikeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	// set bike mesh
	//_player->SetBikeMesh();

	_playerController = Cast<ABikePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	_questionManager = Cast<AQuestionManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AQuestionManager::StaticClass()));

	_correctNum = 0;
	_wrongNum = 0;
	_questionIndex = 0;
	CurrentState = QuestionGameState::Playing;

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
	bool Result = _questionManager->CheckPlayerAnswerInLastRandom(questionID, answer);
	
	if (Result)
	{
		// 正解の数を計算
		_correctNum++;
		// SE
		UGameplayStatics::PlaySound2D(GetWorld(), CorrectSound);
	}
	else
	{
		// 不正解の数を計算
		_wrongNum++;
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

	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	// ゲームオーバー
	if (IsGameFailed())
	{
		CurrentState = QuestionGameState::Failed;
		// UIの表示
		UKismetSystemLibrary::PrintString(this, "GameOver!", true, false, FColor::Red, 10.f);
		// デフォルト入力を無効
		//GameInstance->GetDeviceManager()->DisableDefaultActions();
		//GameInstance->GetDeviceManager()->DisConnectAllDevices();
		// すべての問題を無効にする
		DisableAllQuestions();
		// クイズを記録
		GameInstance->SaveQuizsForResult(_questions);
		// 結果を記録
		GameInstance->SetGameResult(false);
		// ゴールをプレイヤーの進行先に置く
		PlaceGoal(questionID);
		
		//5秒後に次の世界に行く
		// 注意!!レベル名は間違わないように!!
		/*FTimerDelegate timerDelegate;
		timerDelegate.BindUFunction(this, FName("ChangeLevel"), false);
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, timerDelegate, 5.f, false);*/
	}
	// ゲームクリア
	else if (IsGameClear())
	{
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
	return Result;
}

int AQuestionGameMode::GetCurrectNumber() const
{
	return _correctNum;
}

int AQuestionGameMode::GetWrongNumber() const
{
	return _wrongNum;
}

void AQuestionGameMode::GetAllQuestions()
{
	// 問題管理者から問題をゲット
	if (!_questionManager)
	{
		UE_LOG(LogTemp, Error, TEXT("question Manager is not found!!"));
		return;
	}

	// 問題の総数はゲームオーバーになる不正解数とゲームクリアになる正解数の合計
	_questions = _questionManager->GetQuizs(_failCondition + _successCondition);
	if (_questions.Num() <= 0)
		UE_LOG(LogTemp, Error, TEXT("Get questions failed!!"));
}

FQuestion* AQuestionGameMode::GetQuestion()
{
	if (_questions.Num() > 0)
	{
		return _questions[_questionIndex++];
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
	return _wrongNum >= _failCondition;
}

bool AQuestionGameMode::IsGameClear() const
{
	return _correctNum >= _successCondition;
}

void AQuestionGameMode::UpdateAnswerUI()
{
	OnUpdateAnswerUIDelegate.Broadcast(_correctNum, _wrongNum);
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
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestionUIActor::StaticClass(), _questionActors);
	for (AActor* Actor : _questionActors)
	{
		if (AQuestionUIActor* Question = Cast<AQuestionUIActor>(Actor))
		{
			Question->DisableFeature();
		}
	}
}

void AQuestionGameMode::PlaceGoal(int32 questionID)
{
	// 地図上のゴールを探す
	TArray<AActor*> Goals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEndPosition::StaticClass(), Goals);
	if (Goals.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Did not find Goal!!"));
		return;
	}

	AActor* Goal = Goals[0];
	// 問題を特定
	AQuestionUIActor* Target = nullptr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestionUIActor::StaticClass(), _questionActors);
	for (AActor* Actor : _questionActors)
	{
		if (AQuestionUIActor* Question = Cast<AQuestionUIActor>(Actor))
		{
			if (UOptionUIWidget* QuestionUI = Cast<UOptionUIWidget>(Question->GetWidgetComponent()->GetWidget()))
			{
				// UIからもらったquestionIDと比べて目標となっているアクターをゲット
				if (QuestionUI->GetQuestionID() == questionID)
				{
					Target = Question;
					break;
				}
			}
		}
	}
	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("Did not find Question Actor!!"));
		return;
	}
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
