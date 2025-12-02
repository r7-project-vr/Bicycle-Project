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
	/// 左の答えを選ぶ
	/// </summary>
	UFUNCTION()
	void OnSelectLeftAnswer();

	/// <summary>
	/// 左の答えを選ぶ
	/// </summary>
	/// <param name="questionID">問題ID</param>
	/// <param name="answer">答え</param>
	UFUNCTION(BlueprintCallable)
	void SelectLeftAnswer(int questionID, int answer);

	/// <summary>
	/// 右の答えを選ぶ
	/// </summary>
	UFUNCTION()
	void OnSelectRightAnswer();

	/// <summary>
	/// 右の答えを選ぶ
	/// </summary>
	/// <param name="questionID">問題ID</param>
	/// <param name="answer">答え</param>
	UFUNCTION(BlueprintCallable)
	void SelectRightAnswer(int questionID, int answer);

private:

	/// <summary>
	/// 答えを選んだの処理
	/// </summary>
	/// <param name="dir">曲がりたい方向</param>
	void HandleSelectAnswer(FRotator dir);

	/// <summary>
	/// 答えを選ぶ動作を機能させない
	/// </summary>
	void DisableSelectAnswerAction();

	/// <summary>
	/// バイクコンポネント
	/// </summary>
	UBikeComponent* BikeCom;

	/// <summary>
	/// 問題アクター
	/// </summary>
	AQuestionUIActor* QuestionActor;
};
