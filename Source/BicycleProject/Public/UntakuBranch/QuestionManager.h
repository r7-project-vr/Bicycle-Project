// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Question.h"
#include "QuestionManager.generated.h"


/**
 *  Author : WangYunze
 *  Created : 2025-05-24 
 *  Version : v1.3
 */

UCLASS()
class BICYCLEPROJECT_API AQuestionManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuestionManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//DataTableとの接続
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Question")
	UDataTable* QuestionDataTable;

	//Randomで問題を抽選
	UFUNCTION(BlueprintCallable, Category = "Question")
	TArray<FQuestion> GetRandomQuestions(int32 NumQuestions);

	UFUNCTION(BlueprintCallable, Category = "Question")
	bool CheckPlayerAnswerInLastRandom(int32 QuestionID, int32 PlayerAnswer);

	// 2025.09.07 ウー start
	/// <summary>
	/// 一ゲームの全クイズをゲット
	/// </summary>
	/// <param name="NeedAmount">欲しいクイズ数</param>
	/// <returns>クイズ</returns>
	TArray<FQuestion*> GetQuizs(int32 NeedAmount);
	
	// 2025.09.07 ウー end

private:
	void TestRandomQuestions();

	// 2025.09.07 ウー start

	/// <summary>
	/// 問題データをロード
	/// </summary>
	void LoadQuestionsData();

	/// <summary>
	/// 必要とする数の修正
	/// </summary>
	/// <param name="QuizNum">各レベルのクイズの数</param>
	/// <param name="NeedNum">必要な数</param>
	/// <param name="LackSum">足りない数</param>
	void ClampNeedNum(const int& QuizNum, int& NeedNum, int& LackSum);

	/// <summary>
	/// クイズを補充する
	/// </summary>
	/// <param name="LackSum">足りない数</param>
	/// <param name="QuizNum">各レベルのクイズの数</param>
	/// <param name="NeedNum">必要な数</param>
	void ReplenishQuiz(int& LackSum, const int& QuizNum, int& NeedNum);

	/// <summary>
	/// ソースからランダムで一定数のクイズを選ぶ
	/// </summary>
	/// <param name="Quizs">ソースクイズ</param>
	/// <param name="Num">数</param>
	/// <returns>クイズ</returns>
	TArray<FQuestion*> GetRandomQuizsWithNum(TArray<FQuestion*> Quizs, int Num);

	/// <summary>
	/// イージーなクイズ
	/// </summary>
	TArray<FQuestion*> EasyQuizs;

	/// <summary>
	/// ノーマルなクイズ
	/// </summary>
	TArray<FQuestion*> NormalQuizs;

	/// <summary>
	/// ハードなクイズ
	/// </summary>
	TArray<FQuestion*> HardQuizs;

	/// <summary>
	/// 各クイズ難易度の数の割合
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TMap<EQuestionLevel, float> DifficultyQuizRatio;

	/// <summary>
	/// 一ゲームのクイズ
	/// </summary>
	TArray<FQuestion*> QuizsInGame;

	// 2025.09.07 ウー end

	TArray<FQuestion> LastRandomQuestions;
};
