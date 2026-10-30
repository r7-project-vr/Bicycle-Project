// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Actor/WildAnimal.h"
#include "TutorialAnimal.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFadeOutCompleteDelegate);

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API ATutorialAnimal : public AWildAnimal
{
	GENERATED_BODY()
	
public:

	ATutorialAnimal();

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	virtual void Tick(float DeltaTime) override;

	void EnableHighlight_Implementation() override;

	void DisableHighlight_Implementation() override;

	/// <summary>
	/// フェードイン
	/// </summary>
	void FadeIn();

	/// <summary>
	/// フェードアウト
	/// </summary>
	void FadeOut();

	/// <summary>
	/// フェードアウト完了のデリゲート
	/// </summary>
	FFadeOutCompleteDelegate OnFadeOutComplete;

	/// <summary>
	/// 自身のマテリアルを取得
	/// </summary>
	void GetSelfMaterial();

private:

	/// <summary>
	/// ディゾルブ
	/// </summary>
	/// <param name="DeltaTime"></param>
	void Dissolve(float DeltaTime);

	/// <summary>
	/// ディゾルブ値を変更する
	/// </summary>
	/// <param name="Value"></param>
	void ChangeDissolve(float Value);

	/// <summary>
	/// フェードアウトが完了したことを通知する
	/// </summary>
	void NotifyFadeOutComplete();

	/// <summary>
	/// フェードイン/フェードアウトが実行されているかのフラグ
	/// </summary>
	bool bIsFadingInOut;

	/// <summary>
	/// ディゾルブ値
	/// </summary>
	float DissolveValue;

	/// <summary>
	/// ディゾルブの変化速度
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float DissolveSpeed;

	/// <summary>
	/// ディゾルブの最大値
	/// </summary>
	const float MAX_DISSOLVE_VALUE = 0.7f;

	/// <summary>
	/// ディゾルブの最小値
	/// </summary>
	const float MIN_DISSOLVE_VALUE = -0.5f;

	/// <summary>
	/// ディゾルブの目標値
	/// </summary>
	float TargetDissolveValue;

	/// <summary>
	/// マテリアルインスタンス（エフェクトに使う）
	/// </summary>
	UPROPERTY()
	UMaterialInstanceDynamic* DynMaterialInst;

};
