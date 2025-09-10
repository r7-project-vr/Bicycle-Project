// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HeightTesterComponent.generated.h"

class UMyGameInstance;
class UWidgetInteractionComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UHeightTesterComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHeightTesterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	/// <summary>
	/// ゲームインスタンス
	/// </summary>
	UMyGameInstance* GameInstance;

	/// <summary>
	/// 生成物
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* Product;

	/// <summary>
	/// 右手
	/// </summary>
	UWidgetInteractionComponent* RightHand;

	/// <summary>
	/// 左手
	/// </summary>
	UWidgetInteractionComponent* LeftHand;


};
