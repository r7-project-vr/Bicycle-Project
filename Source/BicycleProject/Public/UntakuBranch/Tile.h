// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class ATileManager;
class UBoxComponent;

UCLASS()
class BICYCLEPROJECT_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	//Static Mesh Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	UStaticMeshComponent* TileMesh;

	//Step In Trigger
	UPROPERTY(VisibleAnywhere, Category = "Tile")
	UBoxComponent* TriggerVolume;

	//Set Pointer by TileManager
	void SetManager(class ATileManager* Manager){ TileManager = Manager; }
	
	// 2025.08.01 ウー start
	/// <summary>
	/// マップを生成
	/// </summary>
	/// <param name="IsLeft">true: 左の方, false: 右の方</param>
	void SpawnMap(bool IsLeft);

	/// <summary>
	/// UIの位置、角度を調整
	/// </summary>
	/// <param name="DeltaLocation">移動量</param>
	/// <param name="DeltaRotation">回転量</param>
	void AdjustUI(FVector DeltaLocation = FVector::ZeroVector, FRotator DeltaRotation = FRotator::ZeroRotator);

	/// <summary>
	/// 
	/// </summary>
	void DestroyAll();
	// 2025.08.01 ウー end

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	// Called every frame
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* Overlapped,
		AActor* Other, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& Sweep);


	UPROPERTY()
	ATileManager* TileManager = nullptr;

	// 2025.08.01 ウー start
	/// <summary>
	/// 問題UIを作る
	/// </summary>
	void CreateQuestionUI();

	/// <summary>
	/// 問題UIの位置
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* QuestionSpawnLocation;

	/// <summary>
	/// 問題UIのアクター
	/// </summary>
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AQuestionUIActor> QuestionActor;

	/// <summary>
	/// 生成した問題UI
	/// </summary>
	AQuestionUIActor* QuestionUI;
	// 2025.08.01 ウー end

};
