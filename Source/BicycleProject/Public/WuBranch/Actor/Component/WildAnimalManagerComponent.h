// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WildAnimalManagerComponent.generated.h"

class UBoxComponent;
class AWildAnimal;
class ACharacter;

/**
 * 野生動物のスポーン管理コンポーネント
 * 
 * WildAnimalManagerの子コンポーネントから動物のスポーン位置を自動検出し、
 * 設定された確率に基づいて普通動物とレア動物をランダムにスポーンする。
 * 
 * 主な機能:
 * - 確率ベースの動物選択（普通動物とレア動物の確率の比率）
 * - 再現可能なランダム生成（シード値による制御）
 * - 動的なスポーン位置管理（BoxComponentからの自動検出）
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UWildAnimalManagerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	/// <summary>
	/// コンストラクタ
	/// ランダムストリームの初期化フラグとスポーン位置配列を初期化
	/// </summary>
	UWildAnimalManagerComponent();

protected:
	/// <summary>
	/// ゲーム開始時の初期化処理
	/// 子階層の BoxComponent を検索してスポーン位置として登録し、
	/// ランダムストリームを初期化
	/// </summary>
	virtual void BeginPlay() override;

public:
	/// <summary>
	/// ランダム生成のシード値を設定
	/// 同じシード値を使用することで、動物の出現パターンを再現できる
	/// </summary>
	/// <param name="Seed">シード値（0の場合は自動生成されたシードを使用）</param>
	UFUNCTION(BlueprintCallable, Category = "Wild Animal")
	void SetSeed(int Seed);

	/// <summary>
	/// 動物のスポーンを開始
	/// 確率テーブルを構築し、各スポーン位置にランダムな動物を配置。
	/// スポーン前に既存の動物をすべて削除する。
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Wild Animal")
	void StartSpawnAnimal();

	/// <summary>
	/// スポーンされたすべての動物を削除
	/// SpawnedAnimals配列に登録されているすべての動物を破棄する
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Wild Animal")
	void DestroyAllAnimals();

	/// <summary>
	/// 普通動物のリスト
	/// スポーン可能な普通動物のBlueprintクラスを格納
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Spawn Settings")
	TArray<TSubclassOf<AWildAnimal>> NormalAnimalTypes;

	/// <summary>
	/// レア動物のリスト
	/// スポーン可能なレア動物のBlueprintクラスを格納
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Spawn Settings")
	TArray<TSubclassOf<AWildAnimal>> RareAnimalTypes;

	/// <summary>
	/// 普通動物の確率（確率の比率）
	/// デフォルト: 6.0
	/// 例: 普通動物3匹とレア動物1匹の場合、
	///     普通動物の出現確率は24%、レア動物は4%(24:4=6:1)
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Probability Settings", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float NormalAnimalWeight = 6.0f;

	/// <summary>
	/// レア動物の確率（確率の比率）
	/// デフォルト: 1.0
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wild Animal|Probability Settings", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float RareAnimalWeight = 1.0f;

	/// <summary>
	/// 動物のスポーン位置リスト
	/// BeginPlay時に子階層から検出されたBoxComponentが格納される。
	/// 各BoxComponentの位置が動物のスポーン位置として使用される。
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wild Animal|Spawn Settings")
	TArray<UBoxComponent*> WildAnimalSpawnLocations;

	/// <summary>
	/// スポーン済み動物のリスト
	/// StartSpawnAnimalで生成されたすべての動物への参照を保持する。
	/// DestroyAllAnimals実行時にこのリストを使用して動物を削除する。
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wild Animal")
	TArray<AWildAnimal*> SpawnedAnimals;

private:
	/// <summary>
	/// ランダムストリーム
	/// 再現可能な乱数生成に使用
	/// </summary>
	FRandomStream RandomStream;
	
	/// <summary>
	/// RandomStream が初期化済みかを示すフラグ
	/// 二重初期化を防ぐために使用
	/// </summary>
	bool bRandomStreamInitialized;

	/// <summary>
	/// 動物の確率テーブルエントリ
	/// 各動物の累積確率を保持
	/// </summary>
	struct FAnimalProbabilityEntry
	{
		TSubclassOf<AWildAnimal> AnimalClass;  // 動物クラス
		float CumulativeProbability;           // 累積確率（0～100%）
	};
	
	/// <summary>
	/// 動物の確率テーブル
	/// BuildProbabilityTable で構築され、DecideAnimal で使用されます
	/// </summary>
	TArray<FAnimalProbabilityEntry> ProbabilityTable;

	/// <summary>
	/// 確率テーブルを構築
	/// NormalAnimalTypes と RareAnimalTypes から、
	/// 各動物の累積確率を計算してテーブルを作成します
	/// </summary>
	void BuildProbabilityTable();
	
	/// <summary>
	/// 動物を生成
	/// 指定された位置と回転で動物をスポーンし、
	/// プレイヤーキャラクターで初期化します
	/// </summary>
	/// <param name="Character">プレイヤーキャラクター</param>
	/// <param name="Target">スポーンする動物のクラス</param>
	/// <param name="Location">スポーン位置</param>
	/// <param name="Rotation">スポーン時の回転</param>
	void CreateAnimal(ACharacter* Character, TSubclassOf<AWildAnimal> Target, FVector Location, FRotator Rotation);
	
	/// <summary>
	/// ランダムに動物を決定
	/// 確率テーブルを使用して、重み付けされた確率で動物を選択します
	/// </summary>
	/// <returns>選択された動物のクラス（失敗時は nullptr）</returns>
	TSubclassOf<AWildAnimal> DecideAnimal();
	
	/// <summary>
	/// RandomStream の初期化状態を確認し、必要なら初期化
	/// 未初期化の場合、現在時刻とオブジェクトハッシュから
	/// ランダムなシード値を生成して初期化します
	/// </summary>
	void EnsureRandomStreamInitialized();
};
