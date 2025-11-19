// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/System/FileOperator.h"

FileOperator& FileOperator::GetInstance()
{
	static FileOperator Instance;
	return Instance;
}

FileOperator::FileOperator()
{

}

FileOperator::~FileOperator()
{
}

FString FileOperator::GetFullPath(FString FilePath)
{
	return FPaths::ProjectSavedDir();
}

bool FileOperator::WriteFile(FString FilePath, FString Content)
{
	return FFileHelper::SaveStringToFile(Content, *GetFullPath(FilePath));;
}

bool FileOperator::ReadFile(FString FilePath, FString& oData)
{
	FString FullPath = GetFullPath(FilePath);

	if(!FFileHelper::LoadFileToString(oData, *FullPath))
	{
		return false;
	}

	return true;
}