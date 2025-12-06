// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerSaveGame.generated.h"

/// <summary>
/// 音関連の設定データ
/// </summary>
USTRUCT(BlueprintType)
struct BICYCLEPROJECT_API FSettingVolume
{
    GENERATED_BODY()

    /// <summary>
    /// 全音量
    /// </summary>
    UPROPERTY()
    float Total = 0.5f;

    /// <summary>
    /// BGM
    /// </summary>
    UPROPERTY()
    float BGM = 0.5f;

    /// <summary>
    /// SE音量
    /// </summary>
    UPROPERTY()
    float SE = 0.5f;

    /// <summary>
    /// 動物の鳴き音量
    /// </summary>
    UPROPERTY()
    float Voice = 0.5f;

    /// <summary>
    /// 動物の足音の音量
    /// </summary>
    UPROPERTY()
    float FootSE = 0.5f;
};

/**
 * セーフデータ
 * *構造を変更したら、変えられた構造のデータが変になる
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
    /// 飼っている動物,動物ID
    /// </summary>
    UPROPERTY()
    TArray<int32> OwnedAnimals;

    /// <summary>
    /// 全動物の写真枚数
    /// Map<動物ID, 枚数>
    /// </summary>
    UPROPERTY()
    TMap<int32, int32> AnimalPhotos;

    /// <summary>
    /// 音量
    /// </summary>
    UPROPERTY()
    FSettingVolume Volume;

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