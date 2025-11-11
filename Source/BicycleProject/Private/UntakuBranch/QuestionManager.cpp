// Fill out your copyright notice in the Description page of Project Settings.


#include "UntakuBranch/QuestionManager.h"
#include "WuBranch/MyGameInstance.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"

/**
 *  Author : WangYunze
 *  Created : 2025-05-24 
 *  Version : v1.3
 */

// Sets default values
AQuestionManager::AQuestionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AQuestionManager::BeginPlay()
{

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay triggered!"));
	Super::BeginPlay();

	// 2025.09.07 ウー start
	LoadQuestionsData();
	// 2025.09.07 ウー end
	//TestRandomQuestions();

}

TArray<FQuestion> AQuestionManager::GetRandomQuestions(int32 NumQuestions)
{
	TArray<FQuestion> Result;

	if (!QuestionDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestionDataTable is null!"));
		return Result;
	}


	//Get all row's name
	TArray<FName> RowNames = QuestionDataTable->GetRowNames();

	//Random row's order
	RowNames.Sort([](const FName&, const FName&) {return FMath::RandBool(); });

	//Get pre NumQuestions
	for (int32 i = 0; i < NumQuestions && i < RowNames.Num(); ++i)
	{
		FQuestion* Question = QuestionDataTable->FindRow<FQuestion>(RowNames[i], TEXT(""));
		if (Question)
		{
			Result.Add(*Question);
		}
	}
	LastRandomQuestions = Result; //random結果保存
	
	return Result;
}

// 2025.09.07 ウー start
TArray<FQuestion*> AQuestionManager::GetQuizs(int32 NeedAmount)
{
	TArray<FQuestion*> Result;
	// 各難易度の取得すべき数を計算
	// 基本、イージーとハードを先に計算し、ノーマルは残りの数
	int NeedEasyNum = FMath::RoundToInt((*DifficultyQuizRatio.Find(EQuestionLevel::Easy)) * NeedAmount);
	int NeedHardNum = FMath::RoundToInt((*DifficultyQuizRatio.Find(EQuestionLevel::Hard)) * NeedAmount);
	int NeedNormalNum = NeedAmount - NeedEasyNum - NeedHardNum;

	// 問題数が足りない場合、他の難易度から補充する、優先順位はノーマル > イージー > ハード
	// 各難易度の足りない数
	int LackSum = 0;
	ClampNeedNum(NormalQuizs.Num(), NeedNormalNum, LackSum);
	ClampNeedNum(EasyQuizs.Num(), NeedEasyNum, LackSum);
	ClampNeedNum(HardQuizs.Num(), NeedHardNum, LackSum);

	// 補充する
	ReplenishQuiz(LackSum, NormalQuizs.Num(), NeedNormalNum);
	ReplenishQuiz(LackSum, EasyQuizs.Num(), NeedEasyNum);
	ReplenishQuiz(LackSum, HardQuizs.Num(), NeedHardNum);

	// 必要な数に応じて抽選
	Result.Append(GetRandomQuizsWithNum(EasyQuizs, NeedEasyNum));
	Result.Append(GetRandomQuizsWithNum(NormalQuizs, NeedNormalNum));
	Result.Append(GetRandomQuizsWithNum(HardQuizs, NeedHardNum));

	QuizsInGame = Result;

	return Result;
}
// 2025.09.07 ウー end

void AQuestionManager::TestRandomQuestions()
{
	// 2025.11.12 谷村 start
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Get Game Instance Error!"));
	}
	// 2025.11.12 谷村 end
	else
	{
		TArray<FQuestion> RandomQuestions = GetRandomQuestions(GameInstance->NumOfSets);
		//int32 NumToDraw = 3;//自由設定できる　抽選問題数
		//TArray<FQuestion> RandomQuestions = GetRandomQuestions(NumToDraw);

		int32 MessageKey = 0;

		for (const FQuestion& Q : RandomQuestions)
		{
			//GEngine->AddOnScreenDebugMessage(MessageKey++, 8.f, FColor::Cyan,
			//	FString::Printf(TEXT("[QuestionID: %d] %s"), Q.ID, *Q.Content));

			for (int32 i = 0; i < Q.AnswerContents.Num(); ++i)
			{
				TCHAR OptionChar = 'A' + i;
				//FString OptionStr = FString::Printf(TEXT("%c. %s "), OptionChar, *Q.AnswerContents[i]);
				//GEngine->AddOnScreenDebugMessage(MessageKey++, 8.f, FColor::White, OptionStr);
			}

			//GEngine->AddOnScreenDebugMessage(MessageKey++, 8.f, FColor::Green,
			//	FString::Printf(TEXT("CorrectAnswer: %d"), Q.Correct));
		}
	}
}

bool AQuestionManager::CheckPlayerAnswerInLastRandom(int32 QuestionID, int32 PlayerAnswer)
{
	for (const FQuestion* Q : QuizsInGame)
	{
		if (Q->ID == QuestionID)
		{
			bool bIsCorrect = (Q->Correct == PlayerAnswer);
			UE_LOG(LogTemp, Log, TEXT("Check Last Random Question ID %d: %s"),
				QuestionID, bIsCorrect ? TEXT("Correct") : TEXT("Wrong"));
				return bIsCorrect;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Check Last Random Question ID %d: Not Found!"), QuestionID);
	return false;
}

// 2025.09.07 ウー start
void AQuestionManager::LoadQuestionsData()
{
	// 検査
	if (!QuestionDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestionDataTable is null!"));
		return;
	}

	// クイズデーターを読み込む
	TArray<FQuestion*> Quizs;
	QuestionDataTable->GetAllRows<FQuestion>(TEXT(""), Quizs);

	// 難易度によって分類
	for (FQuestion* Quiz : Quizs)
	{
		if (Quiz->Level == EQuestionLevel::Easy)
		{
			EasyQuizs.Add(Quiz);
		}
		else if (Quiz->Level == EQuestionLevel::Normal)
		{
			NormalQuizs.Add(Quiz);
		}
		else if (Quiz->Level == EQuestionLevel::Hard)
		{
			HardQuizs.Add(Quiz);
		}
	}
}

void AQuestionManager::ClampNeedNum(const int& QuizNum, int& NeedNum, int& LackSum)
{
	if (NeedNum > QuizNum)
	{
		int Over = NeedNum - QuizNum;
		NeedNum = QuizNum;
		LackSum += Over;
	}
}

void AQuestionManager::ReplenishQuiz(int& LackSum, const int& QuizNum, int& NeedNum)
{
	if (LackSum > 0)
	{
		// 残り問題数
		int Remain = FMath::Max(0, QuizNum - NeedNum);
		// 補充いらない
		if (Remain == 0)
			return;

		// 補充する数
		int ReplenishCount = FMath::Min(Remain, LackSum);
		NeedNum += ReplenishCount;
		LackSum -= ReplenishCount;
	}
}

TArray<FQuestion*> AQuestionManager::GetRandomQuizsWithNum(TArray<FQuestion*> Quizs, int Num)
{
	TArray<FQuestion*> Result;

	TArray<int> QuizIndexs;
	// クイズの数が足りない場合
	if (Quizs.Num() <= Num)
	{
		return Quizs;
	}
	else
	{
		for (int Cnt = 0; Cnt < Num; Cnt++)
		{
			int QuizIndex;
			do {
				QuizIndex = FMath::RandRange(0, Quizs.Num() - 1);
			} while (QuizIndexs.Contains(QuizIndex));
			
			Result.Add(Quizs[QuizIndex]);
			QuizIndexs.Add(QuizIndex);
		}
	}
	return Result;
}

// 2025.09.07 ウー end