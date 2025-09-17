// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetInteractionComponent.h"
#include "WidgetInteractionHeadComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BICYCLEPROJECT_API UWidgetInteractionHeadComponent : public UWidgetInteractionComponent
{
	GENERATED_BODY()
	
public:

	UWidgetInteractionHeadComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// UI補助線を表示する
	/// </summary>
	void EnableHintLine();

	/// <summary>
	/// UI補助線を非表示にする
	/// </summary>
	void DisableHintLine();

	/// <summary>
	/// UI補助線がUIを触ったら
	/// </summary>
	/// <param name="WidgetComponent"></param>
	/// <param name="PreviousWidgetComponent"></param>
	UFUNCTION()
	void OnHoverWidget(UWidgetComponent* WidgetComponent, UWidgetComponent* PreviousWidgetComponent);

private:

	/// <summary>
	/// VRデバイスに接続しているか
	/// </summary>
	/// <returns>true: はい / false: いいえ</returns>
	bool IsVRConnect() const;
	
	/// <summary>
	/// 射線
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Line;
};
