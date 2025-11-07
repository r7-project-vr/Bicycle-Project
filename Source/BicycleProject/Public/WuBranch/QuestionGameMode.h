// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "QuestionGameMode.generated.h"

struct FQuestion;
class AQuestionUIActor;

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
	/// <returns>true: 正解, false: 不正解</returns>
	UFUNCTION(BlueprintCallable)
	bool CheckAnswer(int32 questionID, int32 answer);

	/// <summary>
	/// 問題に答えた, クイズなしのバージョン
	/// </summary>
	void AnsweredQuestion();

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
	/// 答え済みか
	/// </summary>
	/// <param name="QuestionID">クイズID</param>
	/// <returns>true: 回答済み, false: 未回答</returns>
	UFUNCTION(BlueprintCallable)
	bool IsAnswered(int32 QuestionID);

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
	/// 成功条件(セット数)をゲット
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	int GetSuccessCondition() const;

	/// <summary>
	/// 成功条件(セット数)をセット
	/// </summary>
	/// <param name="Num">新しい成功数</param>
	UFUNCTION(BlueprintCallable)
	void SetSuccessCondition(int32 Num);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateAnswerRateUIDelegate, int, correct, int, wrong);
	
	UPROPERTY(BlueprintAssignable)
	FUpdateAnswerRateUIDelegate OnUpdateAnswerUIDelegate;

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool isFinish);

private:

	enum QuestionGameState
	{
		Playing,
		Successed,
		Failed,
	};

	/// <summary>
	/// ゲーム中に使う全部の問題を一気にゲット
	/// </summary>
	void GetAllQuestions();

	/// <summary>
	/// UIを更新
	/// </summary>
	void UpdateAnswerUI();

	/// <summary>
	/// ゲーム終了の処理, クイズなしのバージョン
	/// </summary>
	/// <param name="GameResult">true: クリア, false: 失敗</param>
	void EndGame(bool GameResult);

	/// <summary>
	/// ゲームクリアの処理
	/// </summary>
	/// <param name="questionID">新しい成功数</param>
	void HandleGameSuccess(int32 questionID);

	/// <summary>
	/// ゲーム失敗の処理
	/// </summary>
	void HandleGameFailed(int32 questionID);

	/// <summary>
	/// 次のレベルに移動
	/// </summary>
	/// <param name="IsSucc">クリアしたか</param>
	UFUNCTION()
	void ChangeLevel(bool IsSucc);

	/// <summary>
	/// すべての問題を無効にする
	/// </summary>
	void DisableAllQuestions();

	/// <summary>
	/// ゴールを設置
	/// </summary>
	void PlaceGoal(int32 QuestionID = -1);

	/// <summary>
	/// ゴールを見つかる
	/// </summary>
	/// <returns></returns>
	AActor* FindGoal();

	/// <summary>
	/// クイズのIDでクイズを探す
	/// </summary>
	/// <param name="QuestionID">クイズのID</param>
	AQuestionUIActor* FindQuestion(int32 QuestionID);
	AQuestionUIActor* FindQuestion();

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
	class AQuestionManager* QuestionManager;

	/// <summary>
	/// 問題
	/// </summary>
	TArray<FQuestion*> _questions;

	/// <summary>
	/// 問題のインデックス
	/// </summary>
	int QuestionIndex;

	/// <summary>
	/// 何問間違ったらゲームオーバー
	/// </summary>
	UPROPERTY(EditDefaultsOnly)
	int FailCondition;

	/// <summary>
	/// 何問正解したらゲームクリア
	/// </summary>
	UPROPERTY(EditDefaultsOnly)
	int SuccessCondition;

	/// <summary>
	/// 正解の数
	/// </summary>
	int CorrectNum;

	/// <summary>
	/// 間違ってる数
	/// </summary>
	int WrongNum;

	/// <summary>
	/// 今のゲーム状態
	/// </summary>
	QuestionGameState CurrentState;

	/// <summary>
	/// クリアした後に行くマップ
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UWorld> LoadSuccLevel;

	/// <summary>
	/// 失敗した後に行くマップ
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UWorld> LoadFailLevel;

	/// <summary>
	/// 正解のSE
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USoundBase* CorrectSound;

	/// <summary>
	/// 間違ったのSE
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USoundBase* WrongSound;
};
