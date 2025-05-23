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

void AQuestionGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	_player = Cast<ABikeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	// set bike mesh
	//_player->SetBikeMesh();

	_playerController = Cast<ABikePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestionUIActor::StaticClass(), _questionActors);

	_correctNum = 0;
	_wrongNum = 0;

	GetAllQuestions();
}

void AQuestionGameMode::PassTheGoal(AActor* passedActor)
{
	if (_player && passedActor == _player)
	{
		// 自転車への制御を強制的にオフにする
		if (_playerController)
		{
			_playerController->SetPlayerEnabledState(false);
		}

		// ゴールに到達したらゲームクリア
		GameOver(true);

		// エフェクト
		// 花火を探す動作は一旦こちに移動、なぜか二回目世界をリロードする以降BeginPlayでは花火は見つからない
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFirework::StaticClass(), _fireworks);
		for (AActor* firework : _fireworks)
		{
			AFirework* fireworkActor = Cast<AFirework>(firework);
			if (fireworkActor)
			{
				fireworkActor->Fire();
			}
		}

		//5秒後に次の世界に行く
		// 注意!!レベル名は間違わないように!!
		/*FTimerDelegate timerDelegate;
		timerDelegate.BindUFunction(this, FName("ChangeLevel"), "TestMap");
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, timerDelegate, 5.f, false);*/
	}
}

void AQuestionGameMode::CheckAnswer(int32 questionID, int32 answer)
{
	// 問題システムに問題IDと解答を送って答えをもらう
	bool result = rand() % 2 == 0;
	// 正解と不正解の数を計算
	if (result)
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
	UKismetSystemLibrary::PrintString(this, "correct: " + FString::FromInt(_correctNum) + ", wrong: " + FString::FromInt(_wrongNum), true, false, FColor::Blue, 10.f);

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

		//5秒後に次の世界に行く
		// 注意!!レベル名は間違わないように!!
		/*FTimerDelegate timerDelegate;
		timerDelegate.BindUFunction(this, FName("ChangeLevel"), "TestMap");
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, timerDelegate, 5.f, false);*/
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
}

int AQuestionGameMode::GetCurrectNumber() const
{
	return _correctNum;
}

int AQuestionGameMode::GetWrongNumber() const
{
	return _wrongNum;
}

bool AQuestionGameMode::GetAllQuestions()
{
	// 問題システムを呼ぶ
	// 未完成
	//_questions = ;
	return false;
}

FQuestion* AQuestionGameMode::GetQuestion()
{
	if (_questions.Num() > 0)
	{
		return &_questions[0];
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
	onUpdateAnswerUIDelegate.Broadcast(_correctNum, _wrongNum);
}

void AQuestionGameMode::ChangeLevel(FString levelName)
{
	// 今の世界を再読み込み
	if (levelName.Len() != 0)
	{
		// レベルが存在するかどうかをチェック
		UGameplayStatics::OpenLevel(GetWorld(), FName(levelName));
	}
}

void AQuestionGameMode::DisableAllQuestions()
{
	for (AActor* actor : _questionActors)
	{
		if (AQuestionUIActor* question = Cast<AQuestionUIActor>(actor))
		{
			question->DisableFeature();
		}
	}
}

void AQuestionGameMode::PlaceGoal(int32 questionID)
{
	// 地図上のゴールを探す
	TArray<AActor*> goals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEndPosition::StaticClass(), goals);
	if (goals.Num() <= 0)
		return;

	AActor* goal = goals[0];
	// 問題を特定
	AQuestionUIActor* target = nullptr;
	for (AActor* actor : _questionActors)
	{
		if (AQuestionUIActor* question = Cast<AQuestionUIActor>(actor))
		{
			// 王さんを待つ
			//if (UOptionUIWidget* questionUI = Cast<UOptionUIWidget>(question->GetWidget()))
			//{
			//	// UIからもらったquestionIDと比べて目標となっているアクターをゲット
			//	if(questionUI->questionID == questionID)
			//		target = question;
			//}
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
		float distance = 8000.0f;
		goal->SetActorLocation(startLocation + forward * distance);
	}

}
