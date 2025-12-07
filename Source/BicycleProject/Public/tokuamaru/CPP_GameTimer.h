// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include <Components/TextBlock.h>
#include "CPP_GameTimer.generated.h"
class UMyGameInstance;
class AQuestionGameMode;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UCPP_GameTimer : public UUserWidget
{
	GENERATED_BODY()
public:
    /// <summary>
    /// ウィジェットが登録された後の初期化
    /// </summary>
    virtual void NativeOnInitialized() override;

    /// <summary>
    /// 描画の初期化
    /// </summary>
    virtual void NativeConstruct() override;

    UCPP_GameTimer(const FObjectInitializer& ObjectInitializer);

    /// <summary>
    /// 時間の減算
    /// </summary>
    /// <param name="delta"></param>
    UFUNCTION(BlueprintCallable)
    void DecreaseGameTime(float delta);

    /// <summary>
    /// テキスト変更
    /// </summary>
    void SetText();

    void SoundUpDate();



protected:

    bool IsGameScene = true;

    float TotalGameTime = 0.0f;

    float CurrentGameTime = 0.0f;

    /// <summary>
    /// 1ステージ何分か
    /// </summary>
    UPROPERTY(BlueprintReadOnly, Category = "Game")
    float StandardStageTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundBase* SignalSound;

    UPROPERTY(EditAnywhere, Category = "Game")
    float SoundInterval;

    UPROPERTY(BlueprintReadOnly, Category = "Game")
    AQuestionGameMode* QuestionGameMode;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimerText;

    int32 LastPlayedSecond;



	
};
