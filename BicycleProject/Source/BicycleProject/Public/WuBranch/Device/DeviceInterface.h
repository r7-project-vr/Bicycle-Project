// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DeviceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDeviceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BICYCLEPROJECT_API IDeviceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/// <summary>
	/// �l���擾
	/// </summary>
	/// <returns>�l</returns>
	virtual float GetValue();

	/// <summary>
	/// �f�o�C�X�ƃ����N����
	/// </summary>
	/// <returns>true: ���� / false: ���s</returns>
	virtual bool Connect();

	/// <summary>
	/// �f�o�C�X�Ƃ̃����N���O��
	/// </summary>
	/// <returns>true: ���� / false: ���s</returns>
	virtual bool Disconnect();
};
