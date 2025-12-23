// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "MyGameInstance.generated.h"

class UDeviceManager;
struct FQuestion;
class AAnimal;
struct FPlayerSaveGame;

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UMyGameInstance();

	virtual void Init() override;

#pragma region デバイス
public:

	/// <summary>
	/// デバイスマネージャーを取得
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, Category = "Device")
	UDeviceManager* GetDeviceManager() const;

private:

	/// <summary>
	/// デバイスマネージャー
	/// </summary>
	UPROPERTY()
	UDeviceManager* DeviceManager;
#pragma endregion

#pragma region コイン
public:

	/// <summary>
	/// コインを取得
	/// </summary>
	/// <returns></returns>
	int GetTotalCoins() const;

	/// <summary>
	/// コインを設定
	/// </summary>
	/// <param name="Amount">新しいコイン数</param>
	void SetTotalCoins(int NewCoin);

	/// <summary>
	/// 一ゲームあたりにもらえるコインを追加
	/// </summary>
	/// <param name="Amount">追加量</param>
	void AddCoinsPerGame(int Amount);

	/// <summary>
	/// 一ゲームあたりにもらえるコインをリセット
	/// </summary>
	void ResetCoinsPerGame();

	/// <summary>
	/// 一ゲームあたりにもらえるコインを取得
	/// </summary>
	/// <returns></returns>
	int GetCoinsPerGame() const;

	/// <summary>
	/// コインの高さを取得
	/// </summary>
	/// <returns>高さ</returns>
	UFUNCTION(BlueprintCallable)
	float GetCoinHeight() const;

	/// <summary>
	/// コインの高さを設定
	/// </summary>
	/// <param name="Height">高さ</param>
	UFUNCTION(BlueprintCallable)
	void SetCoinHeight(float Height);

	/// <summary>
	/// コインの高さをリセット
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ResetCoinHeight();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateCoinDelegate, int, Num);

	/// <summary>
	/// コインの更新通知
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FUpdateCoinDelegate OnUpdateCoin;

	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateCoinHeightDelegate, float, Height);

	/// <summary>
	/// コインの高さ更新通知
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FUpdateCoinHeightDelegate OnUpdateCoinHeight;

private:

	/// <summary>
	/// コインをファイルに保存
	/// </summary>
	void SaveCoinsToFile(FPlayerSaveGame& Data);

	/// <summary>
	/// ファイルからコインを読み込む
	/// </summary>
	void ReadCoinFromFile(const FPlayerSaveGame& Data);

	/// <summary>
	/// コインの数を更新
	/// </summary>
	void UpdateCoin();

	/// <summary>
	/// コインの高さを更新
	/// </summary>
	void UpdateCoinHeight();

	/// <summary>
	/// 持ってるコイン
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int TotalCoins;

	/// <summary>
	/// 一ゲームあたりにもらえるコインの数
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int CoinsPerGame;

	// コインの高さ
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CoinHeight;
#pragma endregion
	
#pragma region ゲーム結果
public:

	/// <summary>
	/// リザルトのためにクイズを保存
	/// </summary>
	/// <param name="Result"></param>
	void SaveQuizsForResult(TArray<FQuestion*> Result);

	/// <summary>
	/// 一ゲームの結果を記録
	/// </summary>
	/// <param name="Result">true: クリア, false: 失敗</param>
	void SetGameResult(bool bResult);

private:

	/// <summary>
	/// 答えたクイズ
	/// </summary>
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FQuestion> Quizs;

	/// <summary>
	/// ゲームクリアのフラグ
	/// </summary>
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsClear;
#pragma endregion
	
#pragma region RPM
public:

	/// <summary>
	/// 回転数の標準値をセット
	/// </summary>
	/// <param name="Value">新しい値</param>
	UFUNCTION(BlueprintCallable)
	void SetStandardRPM(int Value);

	/// <summary>
	/// 回転数の標準値をゲット
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	int GetStandardRPM() const;

	/// <summary>
	/// 回転数の標準値をリセット
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ResetStandardRPM();

	/// <summary>
	/// RPMの閾値を変更
	/// </summary>
	/// <param name="Value">変更値</param>
	UFUNCTION(BlueprintCallable)
	void AdjustThreshold(int Value);

	/// <summary>
	/// RPMの閾値をゲット
	/// </summary>
	/// <returns>閾値</returns>
	UFUNCTION(BlueprintCallable)
	int GetThreshold() const;

	/// <summary>
	/// RPMの閾値をリセット
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ResetThreshold();

	/// <summary>
	/// 危険値をゲット
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	int GetDangerRPM() const;

	/// <summary>
	/// 安全値をゲット
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	int GetSafeRPM() const;

	/// <summary>
	/// 最大RPMをゲット
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	int GetMaxRPM() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUpdateRPMDelegate, int, Standard, int, Danger, int, Safe);

	/// <summary>
	/// RPMが更新した通知
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FUpdateRPMDelegate OnUpdateRPM;

private:

	/// <summary>
	/// RPMが更新したのを通知
	/// </summary>
	void NotifyUpdateRPM();

	/// <summary>
	/// RPMデータをファイルに保存
	/// </summary>
	/// <param name="Data"></param>
	void SaveRPMToFile(FPlayerSaveGame& Data);

	/// <summary>
	/// ファイルからRPMデータをゲット
	/// </summary>
	/// <param name="Data"></param>
	void ReadRPMFromFile(const FPlayerSaveGame& Data);

	/// <summary>
	/// 回転数の標準値
	/// </summary>
	int StandardRPM;

	/// <summary>
	/// RPMの閾値
	/// </summary>
	int RPMThreshold;

	/// <summary>
	/// 回転数の最大値
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int MaxStandardRPM;

#pragma endregion
	
// 2025.11.12 谷村 start
#pragma region セット数
public:
	/// <summary>
	/// セット数をセット
	/// </summary>
	/// <param name="Value">新しい値</param>
	UFUNCTION(BlueprintCallable)
	void SetNumOfSets(int Value);

	/// <summary>
	/// セット数をゲット
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	int32 GetNumOfSets() const;

private:

	// 2025.12.05 ウー start
	/// <summary>
	/// セット数をファイルに保存
	/// </summary>
	/// <param name="Data"></param>
	void SaveSetsToFile(FPlayerSaveGame& Data);

	/// <summary>
	/// ファイルからセット数をゲット
	/// </summary>
	/// <param name="Data"></param>
	void ReadSetsFromFile(const FPlayerSaveGame& Data);
	// 2025.12.05 ウー end

protected:  // private から protected に変更

	/// <summary>
	/// セット数
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 NumOfSets;
#pragma endregion
// 2025.11.12 谷村 end

#pragma region ペット
public:

	/// <summary>
	/// 動物を追加
	/// </summary>
	/// <param name="AnimalID">対象動物ID</param>
	UFUNCTION(BlueprintCallable)
	void AddAnimal(int32 AnimalID);

	/// <summary>
	/// 動物を削除
	/// </summary>
	/// <param name="Animal">対象動物ID</param>
	void RemoveAnimal(int32 AnimalID);

	/// <summary>
	/// 全動物をゲット
	/// </summary>
	/// <returns>全動物</returns>
	TArray<TSubclassOf<AAnimal>> GetAnimals() const;
	TMap<int32, int32> GetOwnedAnimals() const;

	/// <summary>
	/// 動物IDによる動物の数をゲット
	/// </summary>
	/// <param name="AnimalID">動物ID</param>
	/// <returns>持っている数</returns>
	UFUNCTION(BlueprintCallable)
	int GetAnimalNumByID(int32 AnimalID) const;

	/// <summary>
	/// ついてこれる動物の数を設定
	/// </summary>
	/// <param name="Amount">数</param>
	void SetMaxAnimalCount(int Amount);

	/// <summary>
	/// すでに最大量の動物を持っているか
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	bool HasMaxAnimals() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateAnimalNumDelegate, int32, AnimalID, int, Nums);

	/// <summary>
	/// 持っている動物の数が更新した通知
	/// </summary>
	UPROPERTY(BlueprintAssignable)
	FUpdateAnimalNumDelegate OnUpdateAnimalNum;

private:
	
	/// <summary>
	/// 持っている動物の数が更新したのを通知
	/// </summary>
	/// <param name="AnimalID">動物ID</param>
	/// <param name="Nums">数</param>
	void NotifyUpdateAnimalNum(int32 AnimalID);
	void NotifyUpdateAnimalNum(int32 AnimalID, int Nums);

	/// <summary>
	/// 動物をファイルに保存
	/// </summary>
	void SaveAnimalToFile(FPlayerSaveGame& Data);

	/// <summary>
	/// ファイルから動物をゲット
	/// </summary>
	/// <param name="Data"></param>
	void ReadAnimalFromFile(const FPlayerSaveGame& Data);

	/// <summary>
	/// 一ゲーム内でついてくる動物
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AAnimal>> Animals;

	/// <summary>
	/// プレイヤーが持っている動物
	/// <動物ID, 数>
	/// </summary>
	TMap<int32, int32> OwnedAnimals;

	/// <summary>
	/// 最大ついて来れる動物の数
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int MaxAnimalCount;
#pragma endregion

#pragma region 写真
public:

	/// <summary>
	/// 動物の写真を追加
	/// </summary>
	/// <param name="AnimalID">動物ID</param>
	/// <param name="Nums">枚数</param>
	void AddAnimalPhoto(int32 AnimalID, int32 Nums);

	/// <summary>
	/// 持っている動物の写真の数をゲット
	/// </summary>
	/// <param name="AnimalID">動物ID</param>
	/// <returns>写真の数</returns>
	UFUNCTION(BlueprintCallable)
	int GetAnimalPhotoNum(int32 AnimalID) const;

	/// <summary>
	/// 動物の写真をリセット
	/// </summary>
	void ResetAnimalPhoto();

private:

	/// <summary>
	/// 写真枚数をファイルに保存
	/// </summary>
	/// <param name="Data"></param>
	void SavePhotoToFile(FPlayerSaveGame& Data);

	/// <summary>
	/// ファイルから写真枚数をゲット
	/// </summary>
	/// <param name="Data"></param>
	void ReadPhotoFromFile(const FPlayerSaveGame& Data);

	/// <summary>
	/// 動物の写真数
	/// </summary>
	TMap<int32, int32> AnimalPhotoNums;

#pragma endregion

#pragma region 自転車調整
public:

	/// <summary>
	/// 自転車の偏移量を設定
	/// </summary>
	/// <param name="Offset"></param>
	void SetBikeOffset(FVector Offset);

	/// <summary>
	/// 自転車の偏移量をゲット
	/// </summary>
	/// <returns></returns>
	FVector GetBikeOffset() const;

private:

	/// <summary>
	/// 自転車の偏移量
	/// </summary>
	FVector BikeOffset;
#pragma endregion

#pragma region セーブ
public:

	/// <summary>
	/// すべてのデータをセーブ
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SaveAllToFile();

private:
	
	/// <summary>
	/// すべてのデータを読み込む
	/// </summary>
	void ReadAll();

	/// <summary>
	/// セーフ完了
	/// </summary>
	/// <param name="SlotName">ファイル</param>
	/// <param name="UserIndex"></param>
	/// <param name="bResult">保存結果</param>
	UFUNCTION()
	void OnSaveComplete(bool bResult);

	/// <summary>
	/// ロード完了
	/// </summary>
	/// <param name="SlotName">ファイル</param>
	/// <param name="UserIndex"></param>
	/// <param name="Data">ロードデータ</param>
	UFUNCTION()
	void OnLoadComplete(const FPlayerSaveGame& Data);

	/// <summary>
	/// ファイル名
	/// </summary>
	FString FileName;
#pragma endregion

public:
	/// <summary>
	/// スクリーンショットをキャプチャ
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Screenshot")
	void CaptureVRScreenshot();

	/// <summary>
	/// 最後のスクリーンショットを取得
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, Category = "Screenshot")
	UTexture2D* GetLastScreenshot() const;

	/// <summary>
	/// スクリーンショットの枚数を取得
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, Category = "Screenshot")
	int32 GetScreenshotCount() const;

	/// <summary>
	/// インデックス指定でスクリーンショットを取得
	/// </summary>
	/// <param name="Index">インデックス</param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, Category = "Screenshot")
	UTexture2D* GetScreenshotAtIndex(int32 Index) const;

	/// <summary>
	/// 最後のスクリーンショットを3D空間に配置
	/// </summary>
	/// <param name="PlayerLocation">プレイヤーの位置</param>
	/// <param name="PlayerForward">プレイヤーの前方向</param>
	/// <returns>生成したアクター</returns>
	UFUNCTION(BlueprintCallable, Category = "Screenshot")
	class AScreenshotDisplayActor* DisplayLastScreenshot(FVector PlayerLocation, FVector PlayerForward);

private:
	
	/// <summary>
	/// キャプチャしたスクリーンショットの配列
	/// </summary>
	UPROPERTY()
	TArray<UTexture2D*> CapturedScreenshots;

	/// <summary>
	/// スクリーンショット表示用のアクタークラス
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AScreenshotDisplayActor> ScreenshotDisplayActorClass;
};
