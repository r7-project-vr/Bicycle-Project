// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WuBranch/Enum/TutorialType.h"
#include "TutorialActor.generated.h"

class UTutorialTextPanelWidget;
class UWidgetComponent;

UCLASS()
class BICYCLEPROJECT_API ATutorialActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// <summary>
	/// 初期化
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void Initialize();

	/// <summary>
	/// メッセージの内容を初期化
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void InitMessages();

	/// <summary>
	/// ステップを初期化
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void InitStep();

	/// <summary>
	/// 対象からTutorialTextPanelWidgetを取得
	/// </summary>
	/// <param name="Widget">対象</param>
	/// <returns>TutorialTextPanelWidget</returns>
	UFUNCTION(BlueprintCallable)
	UTutorialTextPanelWidget* GetTutorialTextPanelWidget(UWidgetComponent* Widget);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// チュートリアル開始
	/// </summary>
	UFUNCTION(BlueprintNativeEvent)
	void StartTutorial();

	/// <summary>
	/// チュートリアル終了
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishTutorial();

	/// <summary>
	/// チュートリアルタイプを取得
	/// </summary>
	/// <returns></returns>
	ETutorialType GetTutorialType() const;

private:

	/// <summary>
	/// 現在のメッセージを取得
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	FString GetCurrentMessage();

	/// <summary>
	/// 次のステップに進む
	/// </summary>
	/// <returns>true: 進んだ、false: 終了</returns>
	UFUNCTION(BlueprintCallable)
	bool GotoNextStep();

	/// <summary>
	/// ステップをリセット
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ResetStep();

	/// <summary>
	/// タイプ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	ETutorialType TutorialType;

	/// <summary>
	/// チュートリアルのメッセージ
	/// </summary>
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FString> Messages;

	/// <summary>
	/// ステップ
	/// </summary>
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int CurrentStep;
};
