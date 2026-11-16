// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LaserPointerComponent.generated.h"

class UWidgetInteractionComponent;
class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API ULaserPointerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULaserPointerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// レーザーの強制表示を設定
	/// </summary>
	/// <param name="bNewVisible"></param>
	UFUNCTION(BlueprintCallable)
	void SetForceLaserVisible(bool bNewVisible);

private:

	/// <summary>
	/// WidgetInteractionを探す
	/// </summary>
	UWidgetInteractionComponent* FindInteraction();

	/// <summary>
	/// レーザーの長さを変更する
	/// </summary>
	void ChangeLaserLength(bool bIsHitUI, float HitDistance);

	/// <summary>
	/// レーザーの表示設定
	/// </summary>
	void UpdateLaserVisible(bool bIsHitUI, const FHitResult& Hit);

	/// <summary>
	/// レーザーコンポーネント
	/// </summary>
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> LaserComp;

	/// <summary>
	/// レーザーポイントのコンポーネント
	/// </summary>
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> DotComp;

	/// <summary>
	/// 対象のWidgetInteraction
	/// </summary>
	UPROPERTY()
	TObjectPtr<UWidgetInteractionComponent> Interaction;

	/// <summary>
	/// レーザーの太さ
	/// </summary>
	UPROPERTY(EditAnywhere)
	float Thickness = 0.2f;

	/// <summary>
	/// 円の大きさ
	/// </summary>
	UPROPERTY(EditAnywhere)
	float DotSize = 1.0f;

	/// <summary>
	/// 強制起動
	/// </summary>
	bool bForceVisiable;
};
