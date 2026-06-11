// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhotoDisplayComponent.generated.h"

class UTexture2D;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BICYCLEPROJECT_API UPhotoDisplayComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPhotoDisplayComponent();

protected:
	virtual void BeginPlay() override;

public:	
	/// <summary>
	/// リザルトマップでスクリーンショットをグリッド表示
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Screenshot")
	void DisplayScreenshotsInGrid(FVector StartLocation, FVector GridSpacing);

private:

	/// <summary>
	/// スクリーンショット表示用のアクターを作る
	/// </summary>
	UFUNCTION()
	void MakeDisplayActor(FVector Location,const FAnimalPhoto& PhotoData);


	/// <summary>
	/// スクリーンショット表示用のアクタークラス
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AScreenshotDisplayActor> ScreenshotDisplayActorClass;

	// 2026.05.07 ウー start
	/// <summary>
	/// 横３列
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	int PhotoColumns = 3;

	// 2026.05.07 ウー end

	// 2026.06.11 ウー start
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	float DelayTime;
	// 2026.06.11 ウー end
};