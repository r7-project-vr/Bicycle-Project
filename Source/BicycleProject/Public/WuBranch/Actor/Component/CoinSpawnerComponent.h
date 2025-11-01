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

	/// <summary>
	/// コインを削除
	/// </summary>
	/// <param name="Coin">コイン</param>
	void DestroyCoin(ACoin* Coin);

	/// <summary>
	/// すべてのコインを削除
	/// </summary>
	void DestroyCoins();

	/// <summary>
	/// バインドしたものを解除
	/// </summary>
	void CancelDelegate();

private:

	/// <summary>
	/// 指定された数を生成
	/// </summary>
	/// <param name="Num">数</param>
	/// <param name="StartLocation">開始位置</param>
	void Spawn(int Num, float StartLocation);

	/// <summary>
	/// コインの高さを更新
	/// </summary>
	/// <param name="NewHeight">高さ</param>
	UFUNCTION()
	void UpdateCoinHeight(float NewHeight);

	/// <summary>
	/// コインを生成する区域の高さを更新
	/// </summary>
	/// <param name="NewHeight">高さ</param>
	void UpdateSpawnZoneHeight(float NewHeight);

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
	/// 生成始まる位置
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TMap<EQuestionLevel, float> SpawnStartLocations;

	/// <summary>
	/// 各難易度の生成コイン数
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TMap<EQuestionLevel, int> Amount;

	/// <summary>
	/// 生成回数
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int SpawnTimes;

	/// <summary>
	/// 生成したコイン
	/// </summary>
	TArray<ACoin*> Coins;

};
