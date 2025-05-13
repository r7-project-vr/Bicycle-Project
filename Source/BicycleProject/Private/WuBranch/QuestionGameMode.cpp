// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/QuestionGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "WuBranch/Bike/BikeCharacter.h"
#include "WuBranch/Bike/BikeComponent.h"
#include "WuBranch/BikePlayerController.h"
#include "WuBranch/Actor/Firework.h"

void AQuestionGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	_player = Cast<ABikeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	// set bike mesh
	//_player->SetBikeMesh();

	_playerController = Cast<ABikePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

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
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AQuestionGameMode::ChangeLevel, 5.f, false);
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

	if (IsGameFailed())
	{
		// ゲームオーバーUIの表示
		
		//5秒後に次の世界に行く
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AQuestionGameMode::ChangeLevel, 5.f, false);
	}
	else if (IsGameClear())
	{
		// ゲームクリア

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

void AQuestionGameMode::ChangeLevel()
{
	// 今の世界を再読み込み
	FString map = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), FName(map));
}
