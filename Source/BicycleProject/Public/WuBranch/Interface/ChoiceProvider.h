// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ChoiceProvider.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectV2Delegate);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UChoiceProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BICYCLEPROJECT_API IChoiceProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/// <summary>
	/// ���̓�����I�ԃC�x���g���o�C���h����
	/// </summary>
	/// <param name="object">�o�C���h���ꂽ���֐�������I�u�W�F�N�g</param>
	/// <param name="functionName">�֐���</param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	void BindSelectLeftEvent(UObject* object, FName functionName);

	/// <summary>
	/// �E�̓�����I�ԃC�x���g���o�C���h����
	/// </summary>
	/// <param name="object">�o�C���h���ꂽ���֐�������I�u�W�F�N�g</param>
	/// <param name="functionName">�֐���</param>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	void BindSelectRightEvent(UObject* object, FName functionName);

	/// <summary>
	/// ������I�ׂ�悤�ɂ���
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	void EnableSelectAnswerAction();

	/// <summary>
	/// ������I�ׂȂ��悤�ɂ���
	/// </summary>
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Device Interface")
	void DisableSelectAnswerAction();
};
