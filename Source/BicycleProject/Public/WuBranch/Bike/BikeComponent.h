// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BikeComponent.generated.h"


class AQuestionUIActor;
class ATile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UBikeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBikeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// スピードを0まで下げる
	/// </summary>
	void ReduceVelocityTo0();

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

	/// <summary>
	/// オートプレイを起動
	/// </summary>
	/// <param name="actor"></param>
	void EnableAutoPlay(AQuestionUIActor* actor);

	/// <summary>
	/// オートプレイをキャンセル
	/// </summary>
	void DisableAutoPlay();

	/// <summary>
	/// 今オートプレイしているですか
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	UFUNCTION(BlueprintCallable)
	bool GetIsAutoPlay() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateAutoPlayDelegate, bool, IsAutoPlay);
	/// <summary>
	/// オートプレイ
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FUpdateAutoPlayDelegate OnUpdateAutoPlayEvent;

	/// <summary>
	/// オートプレイの際に指定された位置についたの通知
	/// </summary>
	/// <param name=""></param>
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FArriveLocationDelegate, UBikeComponent*, Bike);
	FArriveLocationDelegate OnArrivedLocationEvent;

	/// <summary>
	/// オートプレイする時同期したい座標
	/// </summary>
	/// <param name="pos">座標</param>
	void SetSynchPos(FVector pos);

	/// <summary>
	/// 答えを選んだの処理
	/// </summary>
	/// <param name="dir">曲がりたい方向</param>
	void HandleSelectAnswer(FRotator dir);

	/// <summary>
	/// コインを手に入れた
	/// </summary>
	/// <param name="Amount">数</param>
	void AddCoins(int Amount);

	/// <summary>
	/// ペナルティ中か
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	bool IsInPenalty() const;

	/// <summary>
	/// ペナルティの持続時間をゲット
	/// </summary>
	/// <returns>持続時間</returns>
	float GetPenaltyDuration() const;

private:

	/// <summary>
	/// 慣性の処理
	/// </summary>
	void HandleInertia(float DeltaTime);

	/// <summary>
	/// 移動
	/// </summary>
	/// <param name="direction">移動方向</param>
	UFUNCTION()
	void OnMove(FVector2D direction);

	/// <summary>
	/// 答えを選ぶ動作を機能させない
	/// </summary>
	void DisableSelectAnswerAction();

	/// <summary>
	/// マップを生成
	/// </summary>
	/// <param name="IsLeft">true: 左の方, false: 右の方</param>
	void SpawnMap(bool IsLeft);

	/// <summary>
	/// 今踏んでいるマップを探す
	/// </summary>
	ATile* FindCurrentTile();

	/// <summary>
	/// コインの処理
	/// </summary>
	/// <param name="Result">クイズの結果</param>
	/// <param name="NeedBonus">ボーナスを加算するか</param>
	void HandleCoin(bool Result, bool NeedBonus);

	/// <summary>
	/// オートプレイの更新を通知
	/// </summary>
	void NotifyAutoPlay();

	/// <summary>
	/// ペナルティの処理
	/// </summary>
	void HandlePenalty();

	/// <summary>
	/// ペナルティをキャンセル
	/// </summary>
	void CancelPenalty();

	/// <summary>
	/// スピード
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	float Speed;

	/// <summary>
	/// 移動の入力あるか
	/// </summary>
	bool bHasMovInput;

	/// <summary>
	/// 慣性の速度
	/// </summary>
	FVector _inertiaVelocity;

	/// <summary>
	/// 慣性の減衰
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	float _inertiaDamping;

	/// <summary>
	/// オートプレイ
	/// </summary>
	bool bIsAutoPlay;

	/// <summary>
	/// オートプレイする時同期したい位置
	/// </summary>
	FVector _synchronizePos;

	/// <summary>
	/// 問題アクター
	/// </summary>
	AQuestionUIActor* _questionActor;

	/// <summary>
	/// クイズごと手に入れたコイン
	/// </summary>
	int CoinsOfQuiz;

	/// <summary>
	/// ボーナスコイン
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int BonusCoin;

	/// <summary>
	/// ペナルティ中か
	/// </summary>
	bool bIsPenalty;

	/// <summary>
	/// ペナルティの持続時間
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PenaltyDuration;
};
