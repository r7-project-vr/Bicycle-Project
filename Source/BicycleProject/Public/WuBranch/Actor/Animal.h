
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animal.generated.h"

class USkeletalMeshComponent;
class UCapsuleComponent;
class UAnimalManagerComponent;

UCLASS()
class BICYCLEPROJECT_API AAnimal : public ACharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAnimal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// 今のスピードをゲット
	/// </summary>
	/// <returns>スピード</returns>
	float GetCurrentSpeed() const;

	/// <summary>
	/// 目標を設定
	/// </summary>
	/// <param name="Target">目標</param>
	/// <param name="Manager">管理者</param>
	void Init(ACharacter* Target, UAnimalManagerComponent* Manager);

	/// <summary>
	/// 偏移を変える
	/// </summary>
	/// <param name="Offset">偏移量</param>
	void ChangeOffset(FVector Offset);

private:

	/// <summary>
	/// 追う
	/// </summary>
	void Chase(float DeltaTime);

	/// <summary>
	/// 追うのをあきらめた
	/// </summary>
	/// <returns>true: 諦めた, false: 続ける</returns>
	bool GiveUp();

	/// <summary>
	/// ついている目標
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACharacter> CurrentTarget;

	/// <summary>
	/// 管理者
	/// </summary>
	UAnimalManagerComponent* AnimalManager;

	/// <summary>
	/// 追い始まる距離
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float StartChaseDistance;

	/// <summary>
	/// 追わなくなる距離
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float GiveUpDistance;

	/// <summary>
	/// スピード
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Speed;

	/// <summary>
	/// 対象との相対位置
	/// </summary>
	FVector RelativeOffset;
};
