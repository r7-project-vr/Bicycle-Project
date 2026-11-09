// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Actor/TutorialActor.h"
#include "PhotoTutorial.generated.h"

class ATutorialAnimal;
class UAnimalDataAsset;
class ATutorialIndicator;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API APhotoTutorial : public ATutorialActor
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;

	/// <summary>
	/// チュートリアル用の動物を表示する
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ShowTutorialAnimal();

	/// <summary>
	/// チュートリアル用の動物を非表示する
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void HideTutorialAnimal();

	/// <summary>
	/// 指標を表示する
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ShowIndicator(FVector Target);

	/// <summary>
	/// 指標を非表示する
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void HideIndicator();

	/// <summary>
	/// UIに対する操作を無効にする
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void DisableUIOperation();

	/// <summary>
	/// 写真を撮ったイベントをバインドする
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void BindPhotoTakenEvent();

	/// <summary>
	/// 写真を撮ったイベントをアンバインドする
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void UnbindPhotoTakenEvent();

private:

	/// <summary>
	/// プレイヤーを探す
	/// </summary>
	void FindPlayer();

	/// <summary>
	/// 指標を探す
	/// </summary>
	void FindIndicator();

	/// <summary>
	/// チュートリアル用の野生動物を生成
	/// </summary>
	UFUNCTION()
	void SpawnTutorialAnimal();

	/// <summary>
	/// 動物の位置と方向を取得
	/// </summary>
	/// <param name="OLocation">位置</param>
	/// <param name="ODir">方向</param>
	void GetAnimalLocationAndDir(FVector& OLocation, FRotator& ODir);

	/// <summary>
	/// チュートリアル動物のフェードアウト
	/// </summary>
	UFUNCTION()
	void OnTutorialAnimalFadeOutComplete();

	/// <summary>
	/// UIに対する操作を有効にする
	/// </summary>
	void EnableUIOperation();

	/// <summary>
	/// 写真を撮ったとき
	/// </summary>
	/// <param name="bHasCapturedAnimal"></param>
	UFUNCTION()
	void OnPhotoTaken(bool bHasCapturedAnimal);

	/// <summary>
	/// チュートリアル動物のテンプレート
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ATutorialAnimal> AnimalTemplate;

	/// <summary>
	/// チュートリアル動物のデータ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UAnimalDataAsset* AnimalData;

	/// <summary>
	/// プレイヤー
	/// </summary>
	UPROPERTY()
	ACharacter* Player;

	/// <summary>
	/// 指標
	/// </summary>
	UPROPERTY()
	ATutorialIndicator* Indicator;

	/// <summary>
	/// プレイヤーとの距離
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float AnimalDistanceFromPlayer;

	/// <summary>
	/// プレイヤーとの角度、単位：度（プレイヤーから見て、真正面は0、右側は+、左側は-）
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float AnimalDirection;

	/// <summary>
	/// チュートリアル用の野生動物
	/// </summary>
	UPROPERTY()
	ATutorialAnimal* TutorialAnimal = nullptr;

	/// <summary>
	/// 動物を隠すときの高さ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float HideAnimalHeight = 0.f;

	/// <summary>
	/// 
	/// </summary>
	FScriptDelegate OnPhotoTakenDelegate;
};
