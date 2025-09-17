// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BikeAdjusterComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UBikeAdjusterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBikeAdjusterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// 高さを調整
	/// </summary>
	/// <param name="Amount">調整量</param>
	UFUNCTION(BlueprintCallable)
	void SetHeight(float Amount);

	/// <summary>
	/// 前後位置を調整
	/// </summary>
	/// <param name="Amount">調整量</param>
	UFUNCTION(BlueprintCallable)
	void SetForwardLocation(float Amount);

	/// <summary>
	/// リセット
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ResetOffset();

	/// <summary>
	/// 調整後の相対位置を取得
	/// </summary>
	/// <returns>相対位置</returns>
	FVector GetAdjustedLocation() const;

	/// <summary>
	/// 調整終了するときに隠すか
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	UFUNCTION(BlueprintCallable)
	bool GetNeedHideAfter() const;

	/// <summary>
	/// 自転車の偏移を保存
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SaveBikeOffset();

private:

	/// <summary>
	/// 自転車の偏移をロード
	/// </summary>
	FVector LoadBikeOffset();

	/// <summary>
	/// 自転車のタグ、メッシュを探すために使う
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName BikeTag;

	/// <summary>
	/// 基本座標(相対位置)
	/// </summary>
	FVector BaseLocation;

	/// <summary>
	/// 偏移量
	/// </summary>
	FVector Offset;

	/// <summary>
	/// 隠しするか
	/// </summary>
	bool IsNeedHide;

};
