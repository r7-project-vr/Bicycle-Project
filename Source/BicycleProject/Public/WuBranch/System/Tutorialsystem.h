// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WuBranch/Enum/TutorialType.h"
#include "Tutorialsystem.generated.h"

struct FTutorialData;
class ATutorialActor;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UTutorialsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/// <summary>
	/// 全てのチュートリアルアクターを探す
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void FindAllTutorialActors();

	/// <summary>
	/// チュートリアルを開く
	/// </summary>
	/// <param name="TutorialType">起動したいチュートリアルのタイプ</param>
	UFUNCTION(BlueprintCallable)
	void OpenTutorial(ETutorialType TutorialType);

	/// <summary>
	/// チュートリアルを終了する
	/// </summary>
	///  <param name="TutorialType">終了したいチュートリアルのタイプ</param>
	UFUNCTION(BlueprintCallable)
	void FinishTutorial(ETutorialType TutorialType);

	/// <summary>
	/// チュートリアルデータをゲット
	/// </summary>
	/// <returns>チュートリアルデータ</returns>
	TArray<FTutorialData> GetTutorialDatas() const;

	/// <summary>
	/// チュートリアルのオンオフを設定
	/// </summary>
	/// <param name="bNeed">オンオフ</param>
	void SetNeedTutorial(bool bNeed);

	/// <summary>
	/// チュートリアルは必要か
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	FORCEINLINE bool IsNeedTutorial() const { return bNeedTutorial; }

	/// <summary>
	/// チュートリアルは進行中ですか
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsTutorialInProgress() const { return bIsTutorialInProgress; }

private:

	/// <summary>
	/// チュートリアルのデータをロード
	/// </summary>
	/// <returns></returns>
	bool LoadTutorialDatas();

	/// <summary>
	/// チュートリアルデータ
	/// </summary>
	TArray<FTutorialData> TutorialDatas;

	/// <summary>
	/// チュートリアル用のアクター
	/// </summary>
	TMap<ETutorialType, ATutorialActor*> TutorialActors;

	/// <summary>
	/// チュートリアルが必要かどうかのフラグ
	/// </summary>
	bool bNeedTutorial;

	/// <summary>
	/// チュートリアルが進行中のフラグ
	/// </summary>
	bool bIsTutorialInProgress;
};
