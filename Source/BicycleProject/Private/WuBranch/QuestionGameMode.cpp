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

		// ゴールに到達したらゲームオーバー
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
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("ChangeLevel"), "TestMap");
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, 5.f, false);
	}
}

void AQuestionGameMode::CheckAnswer(bool answer)
{
	// call question manager to check answer
	bool result = rand() % 2 == 0; // 仮の答え合わせ

	if (result)
		_correctNum++;
	else
		_wrongNum++;

	_AnswerResults.Add(result);
	UKismetSystemLibrary::PrintString(this, "correct: " + FString::FromInt(_correctNum) + ", wrong: " + FString::FromInt(_wrongNum), true, false, FColor::Blue, 10.f);

	if (IsGameFailed())
	{
		// ゲームオーバーUIの表示
		UKismetSystemLibrary::PrintString(this, "GameOver!", true, false, FColor::Red, 10.f);
		// デフォルト入力を無効
		UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
		gameInstance->GetDeviceManager()->DisableDefaultActions();
		// すべての問題を無効にする
		DisableAllQuestions();

		//5秒後に次の世界に行く
		/*FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AQuestionGameMode::ChangeLevel, 5.f, false);*/
	}
	else if (IsGameClear())
	{
		// ゲームクリア
		UKismetSystemLibrary::PrintString(this, "GameClear!", true, false, FColor::Green, 10.f);
		// デフォルト入力を無効
		UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
		gameInstance->GetDeviceManager()->DisableDefaultActions();
		// すべての問題を無効にする
		DisableAllQuestions();
	}
}

TArray<bool> AQuestionGameMode::GetAnswerResults()
{
	return _AnswerResults;
}

int AQuestionGameMode::GetCurrectNumber() const
{
	return _correctNum;
}

int AQuestionGameMode::GetWrongNumber() const
{
	return _wrongNum;
}

bool AQuestionGameMode::IsGameFailed() const
{
	return _wrongNum >= _failCondition;
}

bool AQuestionGameMode::IsGameClear() const
{
	return _correctNum >= _successCondition;
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
