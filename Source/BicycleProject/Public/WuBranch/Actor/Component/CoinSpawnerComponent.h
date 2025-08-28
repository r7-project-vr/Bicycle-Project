// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CoinSpawnerComponent.generated.h"

class ACoin;
enum class EQuestionLevel : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UCoinSpawnerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCoinSpawnerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// ランダムのシードを設定する
	/// </summary>
	/// <param name="Seed">シード</param>
	void SetSeed(int Seed);

	/// <summary>
	/// コインを生成
	/// </summary>
	/// <param name="Level">クイズの難易度</param>
	void Spawn(EQuestionLevel Level);

private:

	/// <summary>
	/// 指定された数を生成
	/// </summary>
	/// <param name="Num">数</param>
	void Spawn(int Num);

	/// <summary>
	/// コインのテンプレート
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACoin> CoinTemplate;

	/// <summary>
	/// ランダム生成器
	/// </summary>
	FRandomStream RandomStream;

	/// <summary>
	/// 生成範囲
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FBox SpawnZone;

	/// <summary>
	/// 生成数
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TMap<EQuestionLevel, int> Amount;

};
