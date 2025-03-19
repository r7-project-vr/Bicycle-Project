// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DeviceInterface.h"
#include "DeviceType.h"
#include "DeviceManager.generated.h"

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UDeviceManager : public UObject
{
	GENERATED_BODY()
	
public:

	/// <summary>
	/// ���X�g�Ƀf�o�C�X��ǉ�
	/// </summary>
	/// <param name="type">�f�o�C�X�̃^�C�v</param>
	/// <param name="device">�f�o�C�X�{��</param>
	void AddDevice(EDeviceType type, TScriptInterface<IDeviceInterface> device);

	/// <summary>
	/// ���X�g����f�o�C�X���폜
	/// </summary>
	/// <param name="type">�폜�������f�o�C�X�̃^�C�v</param>
	void DeleteDevice(EDeviceType type);

	/// <summary>
	/// �f�o�C�X���擾
	/// </summary>
	/// <param name="type">�擾�������f�o�C�X�̃^�C�v</param>
	/// <returns>�f�o�C�X</returns>
	IDeviceInterface* GetDevice(EDeviceType type);

private:

	/// <summary>
	/// ���u
	/// </summary>
	TMap<EDeviceType, TScriptInterface<IDeviceInterface>> devices;
};
