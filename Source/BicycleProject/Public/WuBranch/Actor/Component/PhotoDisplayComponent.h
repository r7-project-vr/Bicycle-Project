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
	/// スクリーンショット表示用のアクタークラス
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AScreenshotDisplayActor> ScreenshotDisplayActorClass;
};