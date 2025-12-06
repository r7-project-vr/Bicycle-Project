// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/System/SaveGameManager.h"
#include "WuBranch/Struct/PlayerSaveGame.h"
#include <JsonObjectConverter.h>

void USaveGameManager::LoadFile(FString FilePath)
{
	// ファイルが見つからない、なので新しいのを作る
	if (!FPaths::FileExists(FilePath))
	{
		FPlayerSaveGame Data;
		SaveFile(FilePath, Data);
		NotifyLoadComplete(Data);
		return;
	}

	Async(EAsyncExecution::ThreadPool, [this, FilePath]()
		{
			FString JsonStr;
			if (!FFileHelper::LoadFileToString(JsonStr, *FilePath))
			{
				UE_LOG(LogTemp, Error, TEXT("load file failed"));
				return;
			}

			FPlayerSaveGame LoadedData;
			if (FJsonObjectConverter::JsonObjectStringToUStruct(JsonStr, &LoadedData))
			{
				// ゲームスレッドに戻る
				AsyncTask(ENamedThreads::GameThread, [this, LoadedData]()
					{
						NotifyLoadComplete(LoadedData);
					});
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Convert json string to struct failed"));
			}
		});
}

void USaveGameManager::SaveFile(FString FilePath, const FPlayerSaveGame& Data)
{

	Async(EAsyncExecution::ThreadPool, [this, Data, FilePath]()
		{
			FString OutputString;
			if (!FJsonObjectConverter::UStructToJsonObjectString(Data, OutputString))
			{
				UE_LOG(LogTemp, Error, TEXT("Convert struct to json string failed"));
				return;
			}

			bool bResult = FFileHelper::SaveStringToFile(OutputString, *FilePath);
			AsyncTask(ENamedThreads::GameThread, [this, bResult]()
				{
					NotifySaveComplete(bResult);
				});
		});
}

void USaveGameManager::NotifyLoadComplete(FPlayerSaveGame Data)
{
	if (OnLoadCompleted.IsBound())
		OnLoadCompleted.Broadcast(Data);
}

void USaveGameManager::NotifySaveComplete(bool bResult)
{
	if (OnSaveCompleted.IsBound())
		OnSaveCompleted.Broadcast(bResult);
}
