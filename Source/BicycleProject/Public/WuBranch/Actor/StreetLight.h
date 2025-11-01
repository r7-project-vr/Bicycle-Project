// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WuBranch/Struct/MyTime.h"
#include "StreetLight.generated.h"

class UPointLightComponent;

UCLASS()
class BICYCLEPROJECT_API AStreetLight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStreetLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// 時間の更新処理
	/// </summary>
	/// <param name="NewTime">時間</param>
	UFUNCTION()
	void UpdateTime(FMyTime NewTime);

protected:

	/// <summary>
	/// 電気をつける
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void TurnOn();

private:

	/// <summary>
	/// 電気を消す
	/// </summary>
	void TurnOff();

	/// <summary>
	/// 照明灯のメッシュ
	/// </summary>
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	/// <summary>
	/// 照明灯のライト
	/// </summary>
	UPROPERTY(EditAnywhere)
	UPointLightComponent* Light;

	/// <summary>
	/// 灯る時間
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FMyTime TurnOnTime;

	/// <summary>
	/// 消す時間
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FMyTime TurnOffTime;

	/// <summary>
	/// 明るさ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Intensity;
};
