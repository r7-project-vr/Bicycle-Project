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

	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFirework::StaticClass(), _fireworks);
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

void AQuestionGameMode::ChangeLevel()
{
	// 今の世界を再読み込み
	FString map = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), FName(map));
}
