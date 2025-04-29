// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BikeCharacter.generated.h"

class UBikeComponent;

UCLASS()
class BICYCLEPROJECT_API ABikeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABikeCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// <summary>
	/// バイクのメッシュを変更
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ChangeBikeMesh();

	/// <summary>
	/// スピードを取得
	/// </summary>
	/// <returns>スピード</returns>
	UFUNCTION(BlueprintCallable)
	float GetSpeed();

	/// <summary>
	/// ハンドラーの角度を取得
	/// </summary>
	/// <returns>ハンドラーの角度</returns>
	UFUNCTION(BlueprintCallable)
	float GetHandlerAngle();

private:

	/// <summary>
	/// 自転車メッシュをロード
	/// </summary>
	void LoadBikeMesh();

	/// <summary>
	/// ロード終わったら
	/// </summary>
	void LoadMeshComplete();

	/// <summary>
	/// ロード先の自転車メッシュ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UStaticMesh> _bikeMeshNeedLoad;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<USkeletalMesh> _bikeSkeletalNeedLoad;

	/// <summary>
	/// 実際の自転車メッシュ
	/// </summary>
	UStaticMeshComponent* _bikeMesh;

	/// <summary>
	/// 自転車の機能
	/// </summary>
	UBikeComponent* _bike;

};
