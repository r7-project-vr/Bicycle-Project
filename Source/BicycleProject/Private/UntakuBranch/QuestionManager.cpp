// Fill out your copyright notice in the Description page of Project Settings.


#include "UntakuBranch/QuestionManager.h"
#include "Engine/DataTable.h"


/**
 *  Author : WangYunze
 *  Created : 2025-05-24 
 *  Version : v1.1
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
	Super::BeginPlay();
	
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
	return Result;
}

