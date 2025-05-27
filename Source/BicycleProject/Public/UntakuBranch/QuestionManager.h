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

private:
	void TestRandomQuestions();

	TArray<FQuestion> LastRandomQuestions;
};
