// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WuBranch/Enum/TutorialType.h"
#include "TutorialTrigPoint.generated.h"

class UShapeComponent;

UCLASS()
class BICYCLEPROJECT_API ATutorialTrigPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialTrigPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

private:

	/// <summary>
	/// トリガー範囲を探す
	/// </summary>
	/// <returns>true: 見つかった、false: 見つからない</returns>
	bool FindTrigArea();

	/// <summary>
	/// トリガー範囲に入ったら
	/// </summary>
	UFUNCTION()
	void OnOverlapBeginTriggerArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/// <summary>
	/// トリガー範囲
	/// </summary>
	UShapeComponent* TrigArea;

	/// <summary>
	/// チュートリアルタイプ
	/// </summary>
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	ETutorialType Type;
};
