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

	GetAllQuestions();
}

void AQuestionGameMode::PassTheGoal(AActor* passedActor)
{
	if (_player && passedActor == _player)
	{
		// 自転車への制御を強制的にオフにする
		UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
		GameInstance->GetDeviceManager()->DisableDefaultActions();
		if (_playerController)
		{
			_playerController->SetPlayerEnabledState(false);
		}
		_player->GetBikeComponent()->ReduceVelocityTo0();

		// ゴールに到達したらゲームクリア
		GameOver(true);

		//5秒後に次の世界に行く
		// 注意!!レベル名は間違わないように!!
		FTimerDelegate timerDelegate;
		//timerDelegate.BindUFunction(this, FName("ChangeLevel"), "TitleMap");
		timerDelegate.BindUFunction(this, "ChangeLevel", true);
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, timerDelegate, 5.f, false);
	}
}

bool AQuestionGameMode::CheckAnswer(int32 questionID, int32 answer)
{
	// 問題システムに問題IDと解答を送って答えをもらう
	bool Result = _questionManager->CheckPlayerAnswerInLastRandom(questionID, answer);
	// 正解と不正解の数を計算
	if (Result)
		_correctNum++;
	else
		_wrongNum++;

	// 答えを保存
	// 問題IDがquestionIDの問題を見つける
	FQuestion* question = _questions.FindByPredicate([questionID](const FQuestion& question) {
		return question.ID == questionID;
	});
	if (question)
	{
		question->PlayerAnswer = answer;
	}

	// UI更新
	UpdateAnswerUI();
	//UKismetSystemLibrary::PrintString(this, "correct: " + FString::FromInt(_correctNum) + ", wrong: " + FString::FromInt(_wrongNum), true, false, FColor::Blue, 10.f);

	// ゲームオーバー
	if (IsGameFailed())
	{
		// UIの表示
		UKismetSystemLibrary::PrintString(this, "GameOver!", true, false, FColor::Red, 10.f);
		// デフォルト入力を無効
		UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
		gameInstance->GetDeviceManager()->DisableDefaultActions();
		// すべての問題を無効にする
		DisableAllQuestions();
		// エフェクト

		// 先にデバイスを切断

		//5秒後に次の世界に行く
		// 注意!!レベル名は間違わないように!!
		FTimerDelegate timerDelegate;
		timerDelegate.BindUFunction(this, FName("ChangeLevel"), false);
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, timerDelegate, 5.f, false);
	}
	// ゲームクリア
	else if (IsGameClear())
	{
		// ゲームクリア
		UKismetSystemLibrary::PrintString(this, "GameClear!", true, false, FColor::Green, 10.f);
		// すべての問題を無効にする
		DisableAllQuestions();
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
	_questions = _questionManager->GetRandomQuestions(_failCondition + _successCondition);
	if (_questions.Num() <= 0)
		UE_LOG(LogTemp, Error, TEXT("Get questions failed!!"));
}

FQuestion* AQuestionGameMode::GetQuestion()
{
	if (_questions.Num() > 0)
	{
		return &_questions[_questionIndex++];
	}
	return nullptr;
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
	TArray<AActor*> goals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEndPosition::StaticClass(), goals);
	if (goals.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Did not find Goal!!"));
		return;
	}
		

	AActor* goal = goals[0];
	// 問題を特定
	AQuestionUIActor* target = nullptr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestionUIActor::StaticClass(), _questionActors);
	for (AActor* actor : _questionActors)
	{
		if (AQuestionUIActor* question = Cast<AQuestionUIActor>(actor))
		{
			if (UOptionUIWidget* questionUI = Cast<UOptionUIWidget>(question->GetWidgetComponent()->GetWidget()))
			{
				// UIからもらったquestionIDと比べて目標となっているアクターをゲット
				if (questionUI->GetQuestionID() == questionID)
				{
					target = question;
					break;
				}
			}
		}
	}
	if (!target)
	{
		UE_LOG(LogTemp, Error, TEXT("Did not find Question Actor!!"));
		return;
	}
	// 問題の出口の位置と向きをゲット
	FVector startLocation, forward;
	if (target->GetExitLocationAndForward(startLocation, forward))
	{
		// ゴールを進行先に置く
		float distance = 5000.0f;
		startLocation.Z = 100.f;
		goal->SetActorLocation(startLocation + forward * distance);
		goal->SetActorRotation((forward * -1).Rotation());
	}

}
