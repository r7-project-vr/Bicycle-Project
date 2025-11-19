// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BICYCLEPROJECT_API FileOperator
{
public:

	static FileOperator& GetInstance();

	/// <summary>
	/// データをファイルに書き込む
	/// </summary>
	/// <param name="FilePath">ファイルパス</param>
	/// <param name="Content">データ</param>
	/// <returns>true: 成功, false: 失敗</returns>
	bool WriteFile(FString FilePath, FString Content);

	/// <summary>
	/// ファイルからデータを読み込む
	/// </summary>
	/// <param name="FilePath">ファイルパス</param>
	/// <param name="oData">読み込んだデータ</param>
	/// <returns>true: 成功, false: 失敗</returns>
	bool ReadFile(FString FilePath, FString& oData);

private:

	FileOperator();
	~FileOperator();

public:
	FString GetFullPath(FString FilePath);

private:
	FileOperator(const FileOperator&) = delete;
	FileOperator& operator=(const FileOperator&) = delete;
};
