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

	/// <summary>
	/// 答え合わせ(仮)
	/// </summary>
	/// <param name="question">答え</param>
	UFUNCTION(BlueprintCallable)
	void CheckAnswer(bool answer = false);

	/// <summary>
	/// 今までの回答をゲット
	/// </summary>
	/// <returns>全部の回答</returns>
	UFUNCTION(BlueprintCallable)
	TArray<bool> GetAnswerResults();

	/// <summary>
	/// 正解した答えの数をゲット
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	int GetCurrectNumber() const;

	/// <summary>
	/// 間違ってる答えの数をゲット
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	int GetWrongNumber() const;

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool isFinish);

private:

	/// <summary>
	/// ゲームオーバーのチェック
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	bool IsGameFailed() const;

	/// <summary>
	/// ゲームクリアのチェック
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	bool IsGameClear() const;

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

	/// <summary>
	/// 答えの結果(仮)
	/// 問題の構造ができたらここを変える
	/// </summary>
	TArray<bool> _AnswerResults;

	/// <summary>
	/// 何問間違ったらゲームオーバー
	/// </summary>
	UPROPERTY(EditDefaultsOnly)
	int _failCondition;

	/// <summary>
	/// 何問正解したらゲームクリア
	/// </summary>
	UPROPERTY(EditDefaultsOnly)
	int _successCondition;

	/// <summary>
	/// 正解の数
	/// </summary>
	int _correctNum;

	/// <summary>
	/// 間違ってる数
	/// </summary>
	int _wrongNum;
};
