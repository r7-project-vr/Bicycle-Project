// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HeadLookComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UHeadLookComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHeadLookComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OpenLine();

	void CloseLine();

private:

	/// <summary>
	/// VRデバイスに接続しているか
	/// </summary>
	/// <returns>true: はい / false: いいえ</returns>
	bool IsVRConnect() const;

	UPROPERTY(EditDefaultsOnly, Category = "Head Look", meta = (AllowPrivateAccess = "true"))
	class UWidgetInteractionComponent* _lineOfSight;
};
