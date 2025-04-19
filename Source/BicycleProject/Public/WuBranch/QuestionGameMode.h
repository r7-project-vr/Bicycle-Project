// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "QuestionGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API AQuestionGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;

public:

	/// <summary>
	/// ゴールに到達した
	/// </summary>
	/// <param name="passedActor"></param>
	void PassTheGoal(AActor* passedActor);

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool isFinish);

private:

	/// <summary>
	/// 次のレベルに移動
	/// </summary>
	void ChangeLevel();

	/// <summary>
	/// プレイヤー
	/// </summary>
	class ABikeCharacter* _player;

	/// <summary>
	/// プレイヤコントローラー
	/// </summary>
	class ABikePlayerController* _playerController;

	/// <summary>
	/// レベル上全部の花火
	/// </summary>
	TArray<class AActor*> _fireworks;
};
