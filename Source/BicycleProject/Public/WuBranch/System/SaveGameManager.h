// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Struct/PlayerSaveGame.h"
#include "SaveGameManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadResultDelegate,const FPlayerSaveGame&, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSaveResultDelegate, bool, bResult);

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API USaveGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	/// <summary>
	/// ロードファイル
	/// </summary>
	/// <param name="FilePath">ファイル</param>
	void LoadFile(FString FilePath);

	/// <summary>
	/// セーフファイル
	/// </summary>
	/// <param name="FilePath">ファイル</param>
	/// <param name="Data">保存したいデータ</param>
	/// <param name="Func">コールバック</param>
	void SaveFile(FString FilePath, const FPlayerSaveGame& Data);

	/// <summary>
	/// ロード完了デリケート
	/// </summary>
	FLoadResultDelegate OnLoadCompleted;

	/// <summary>
	/// セーフ完了デリケート
	/// </summary>
	FSaveResultDelegate OnSaveCompleted;

private:

	/// <summary>
	/// ロード完了を通知する
	/// </summary>
	/// <param name="Data">データ</param>
	void NotifyLoadComplete(FPlayerSaveGame Data);

	/// <summary>
	/// セーフ完了を通知する
	/// </summary>
	/// <param name="bResult">結果</param>
	void NotifySaveComplete(bool bResult);
};
