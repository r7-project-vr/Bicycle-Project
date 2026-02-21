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
	/// マップを生成
	/// </summary>
	/// <param name="bIsLeft">true: 左の方, false: 右の方</param>
	void SpawnNextMap(bool bIsLeft);

	/// <summary>
	/// コインの処理
	/// </summary>
	/// <param name="bResult">クイズの結果</param>
	/// <param name="bNeedBonus">ボーナスを加算するか</param>
	void HandleCoin(bool bResult, bool bNeedBonus);

	/// <summary>
	/// コインを手に入れた
	/// </summary>
	/// <param name="Amount">数</param>
	void AddCoins(int Amount);

	/// <summary>
	/// ペナルティを起動
	/// </summary>
	void ApplyPenalty();

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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatePenaltyDelegate, bool, bIsPenalty);

	/// <summary>
	/// ペナルティの更新
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FUpdatePenaltyDelegate OnUpdatePenaltyEvent;

private:

	/// <summary>
	/// 今踏んでいるマップを探す
	/// </summary>
	ATile* FindCurrentTile();

	/// <summary>
	/// ペナルティをキャンセル
	/// </summary>
	void CancelPenalty();

	/// <summary>
	/// ペナルティ状態の更新を通知
	/// </summary>
	void NotifyPenaltyUpdate();

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
