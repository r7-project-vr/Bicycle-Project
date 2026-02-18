// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResponderComponent.generated.h"

class AQuestionUIActor;
class UBikeComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UResponderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UResponderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// クイズのアクターを設定
	/// </summary>
	/// <param name="Quiz">クイズ</param>
	void SetQuiz(AQuestionUIActor* Quiz);

	/// <summary>
	/// クイズのアクターを削除
	/// </summary>
	void DeleteQuiz();

	/// <summary>
	/// 左の答えを選ぶ
	/// </summary>
	UFUNCTION()
	void OnSelectLeftAnswer();

	/// <summary>
	/// 左の答えを選ぶ
	/// </summary>
	/// <param name="QuizID">クイズID</param>
	/// <param name="Answer">答え</param>
	UFUNCTION(BlueprintCallable)
	void SelectLeftAnswer(int QuizID, int Answer);

	/// <summary>
	/// 右の答えを選ぶ
	/// </summary>
	UFUNCTION()
	void OnSelectRightAnswer();

	/// <summary>
	/// 右の答えを選ぶ
	/// </summary>
	/// <param name="QuizID">クイズID</param>
	/// <param name="Answer">答え</param>
	UFUNCTION(BlueprintCallable)
	void SelectRightAnswer(int QuizID, int Answer);

private:

	/// <summary>
	/// 答えを選んだの処理
	/// </summary>
	/// <param name="Dir">曲がりたい方向</param>
	void HandleSelectAnswer(FRotator Dir);

	/// <summary>
	/// 答えを選ぶ動作を機能させない
	/// </summary>
	void DisableSelectAnswerAction();

	/// <summary>
	/// バイクコンポネント
	/// </summary>
	UBikeComponent* BikeCom;

	/// <summary>
	/// クイズのアクター
	/// </summary>
	AQuestionUIActor* QuizActor;
};
