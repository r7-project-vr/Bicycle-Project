// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Device/DeviceVer2.h"
#include "WuBranch/Interface/MoveProvider.h"
#include "KeyboardVer2.generated.h"

/**
 * キーボードの入力なので、プラットフォームがウィンドウズの場合、既に繋がっている状態がデフォルトとして認識します
 * 他のプラットフォームだったら、有線か無線で繋がっているかどうかの判断が必要です
 */
UCLASS()
class BICYCLEPROJECT_API UKeyboardVer2 : public UDeviceVer2, public IMoveProvider
{
	GENERATED_BODY()
	
public:

	UKeyboardVer2();

	bool Connect() override;

	bool DisConnect() override;

	void BindMoveEvent_Implementation(UObject* object, FName functionName) override;
	void EnableMove_Implementation() override;
	void DisableMove_Implementation() override;

private:

	/// <summary>
	/// 
	/// </summary>
	FMoveV2Delegate OnMove;
};
