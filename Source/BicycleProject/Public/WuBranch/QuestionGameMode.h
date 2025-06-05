// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "QuestionGameMode.generated.h"

struct FQuestion;

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
	/// 問題を取得
	/// </summary>
	FQuestion* GetQuestion();

	/// <summary>
	/// ゴールに到達した
	/// </summary>
	/// <param name="passedActor"></param>
	void PassTheGoal(AActor* passedActor);

	/// <summary>
	/// 答え合わせ
	/// </summary>
	/// <param name="questionID">問題ID</param>
	/// <param name="answer">解答</param>
	UFUNCTION(BlueprintCallable)
	void CheckAnswer(int32 questionID, int32 answer);

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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateAnswerRateUIDelegate, int, correct, int, wrong);
	
	UPROPERTY(BlueprintAssignable)
	FUpdateAnswerRateUIDelegate onUpdateAnswerUIDelegate;

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool isFinish);

private:

	/// <summary>
	/// ゲーム中に使う全部の問題を一気にゲット
	/// </summary>
	void GetAllQuestions();

	/// <summary>
	/// UIを更新
	/// </summary>
	void UpdateAnswerUI();

	/// <summary>
	/// 次のレベルに移動
	/// </summary>
	/// <param name="levelName">レベル名</param>
	void ChangeLevel(FString levelName = "");

	/// <summary>
	/// すべての問題を無効にする
	/// </summary>
	void DisableAllQuestions();

	/// <summary>
	/// ゴールを設置
	/// </summary>
	void PlaceGoal(int32 questionID);

	/// <summary>
	/// プレイヤー
	/// </summary>
	class ABikeCharacter* _player;

	/// <summary>
	/// プレイヤコントローラー
	/// </summary>
	class ABikePlayerController* _playerController;

	/// <summary>
	/// 問題の管理者
	/// </summary>
	class AQuestionManager* _questionManager;


	/// <summary>
	/// 問題
	/// </summary>
	TArray<FQuestion> _questions;

	/// <summary>
	/// 問題のインデックス
	/// </summary>
	int _questionIndex;

	/// <summary>
	/// すべての問題Actor
	/// </summary>
	TArray<class AActor*> _questionActors;

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
