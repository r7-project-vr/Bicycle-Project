// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PositionResetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UPositionResetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPositionResetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// 新しい位置をリセット位置として記録する
	/// </summary>
	/// <param name="Pos">位置</param>
	void RecordResetPosition(FVector Pos);

	/// <summary>
	/// 新しい角度をリセット角度として記録する
	/// </summary>
	/// <param name="Angle"></param>
	void RecordResetAngle(FQuat Angle);

	/// <summary>
	/// リセットポイントをゲット
	/// </summary>
	/// <returns>リセットポイント</returns>
	FTransform GetResetPoint() const;

	/// <summary>
	/// 所持者の位置と角度をリセット
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ResetOwnerTransform();

private:

	/// <summary>
	/// リセットポイント
	/// </summary>
	FTransform Point;
};
