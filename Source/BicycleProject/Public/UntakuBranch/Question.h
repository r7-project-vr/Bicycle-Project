// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Question.generated.h"

/**
 *  Author : WangYunze
 *  Created : 2025-05-17 
 *  Version : v1.2
 */

//難易度enum
UENUM(BlueprintType)
enum class EQuestionLevel : uint8
{
    Hard UMETA(DisplayName = "Hard"),
    Normal UMETA(DisplayName = "Normal"),
    Easy UMETA(DisplayName = "Easy")
};

//問題結構体
USTRUCT(BlueprintType)
struct FQuestion : public FTableRowBase
{
    GENERATED_BODY()

    //問題ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Question")
    int32 ID = 0;

    //難易度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Question")
    EQuestionLevel Level = EQuestionLevel::Normal;

    //問題文
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Question")
    FString Content = TEXT("");

    //答え数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Question")
    int32 AnswerNum = 0;

    //答え内容
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Question")
    TArray<FString> AnswerContents;

    //正解
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Question")
    int32 Correct = -1;

    //プレイヤ解答
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Question")
    int32 PlayerAnswer = -1;
    
};