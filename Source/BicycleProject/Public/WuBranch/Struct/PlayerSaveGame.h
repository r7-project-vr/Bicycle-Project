// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerSaveGame.generated.h"

/// <summary>
/// 動物関連のデータ
/// </summary>
USTRUCT(BlueprintType)
struct BICYCLEPROJECT_API FAnimalDatas
{
    GENERATED_BODY()

    /// <summary>
    /// 持っている数
    /// </summary>
    UPROPERTY()
    int32 Count = 0;

    /// <summary>
    /// 解放状況
    /// </summary>
    UPROPERTY()
    float UnlockProgress = 0.0f; 
};

/**
 *
 */
USTRUCT(BlueprintType)
struct BICYCLEPROJECT_API FPlayerSaveGame
{
    GENERATED_BODY()

    /// <summary>
    /// 持っているコイン数
    /// </summary>
    UPROPERTY()
    int32 Coins = 0;

    /// <summary>
    /// 動物関連のデータ
    /// Map<動物ID, データ>
    /// </summary>
    UPROPERTY()
    TMap<int32, FAnimalDatas> AnimalData;

    /// <summary>
    /// 全音量
    /// </summary>
    UPROPERTY()
    float TotalVolume = 1.0f;

    /// <summary>
    /// BGM音量
    /// </summary>
    UPROPERTY()
    float BGMVolume = 1.0f;

    /// <summary>
    /// SE音量
    /// </summary>
    UPROPERTY()
    float SEVolume = 1.0f;

    /// <summary>
    /// 動物の鳴き音量
    /// </summary>
    UPROPERTY()
    float VoiceVolume = 1.0f;

    /// <summary>
    /// 動物の足音の音量
    /// </summary>
    UPROPERTY()
    float FootSEVolume = 1.0f;

    /// <summary>
    /// マップのセット数
    /// </summary>
    UPROPERTY()
    int32 MapSets = 4;

    /// <summary>
    /// 制限した回転数
    /// </summary>
    UPROPERTY()
    float RPMLimit = 50.0f;

    /// <summary>
    /// 回転数の閾値
    /// </summary>
    UPROPERTY()
    int RPMThreshold = 10;
};