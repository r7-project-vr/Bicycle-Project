// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhotoCaptureComponent.generated.h"

class UBoxComponent;
class AWildAnimal;
class USoundBase;
class UWidgetComponent;
class UMyGameInstance;
class UAnimalManagerComponent;
struct FAnimalPhoto;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BICYCLEPROJECT_API UPhotoCaptureComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPhotoCaptureComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// カメラに追従する撮影ボックスを初期化
	/// </summary>
	void InitializePhotoCaptureBox(USceneComponent* AttachParent);

	/// <summary>
	/// スクリーンショット撮影後に呼ばれる
	/// </summary>
	UFUNCTION()
	void OnScreenshotTaken();

	/// <summary>
	/// デバッグ：コリジョン設定を確認
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugPhotoCaptureBox();

	/// <summary>
	/// PhotoCaptureBoxを取得
	/// </summary>
	UBoxComponent* GetPhotoCaptureBox() const { return PhotoCaptureBox; }

private:

	// 2026.05.27 ウー start Tickごとに野良動物を探すの代わりにコリジョンを利用する
	/// <summary>
	/// 範囲に入ったときの処理
	/// </summary>
	UFUNCTION()
	void OnPhotoCaptureBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/// <summary>
	/// 範囲を出た時の処理
	/// </summary>
	UFUNCTION()
	void OnPhotoCaptureBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	// 2026.05.27 ウー end

	/// <summary>
	/// 写真を撮る際に見えないようにするUIを探す
	/// </summary>
	void FindAllNeedCloseUIs();

	/// <summary>
	/// UIの表示/非表示を切り替える
	/// </summary>
	/// <param name="bVisible"></param>
	void SetUIVisibility(bool bVisible);

	/// <summary>
	/// カメラに追従させる
	/// </summary>
	void FollowCamera();

	/// <summary>
	/// 写真を撮る
	/// </summary>
	void TakePhoto(UMyGameInstance* GameInstance, TArray<int32> DetectedPetsID);

	/// <summary>
	/// 撮影時に範囲内の動物を検出
	/// </summary>
	//void DetectAndScoreAnimals();
	TArray<int32> DetectAnimals(UMyGameInstance* GameInstance);

	/// <summary>
	/// 検出した動物の名前をゲット
	/// </summary>
	/// <param name="DetectedPetsID"></param>
	/// <returns></returns>
	FAnimalPhoto MakePhotoDatas(TArray<int32> DetectedPetsID, UTexture2D* Texture);

	/* 2026.06.10 ウー start 使わない
	/// <summary>
	/// 撮影範囲内の動物を検出
	/// </summary>
	void FindCaptureAnimal(TArray<AActor*>& OverlappingActors);

	/// <summary>
	/// 動物のハイライトを有効化
	/// </summary>
	void EnableLightAnimal(TArray<AActor*>& Animals);

	/// <summary>
	/// 動物のハイライトを無効化
	/// </summary>
	void DisableLightAnimal(TArray<AActor*>& Animals);
	*/

	/// <summary>
	/// カメラ追従撮影判定用コリジョン
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* PhotoCaptureBox;

	/// <summary>
	/// 写真撮れたSE
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Photo", meta = (AllowPrivateAccess = "true"))
	USoundBase* TakePhotoSuccSE;

	/// <summary>
	/// 写真撮れないSE
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Photo", meta = (AllowPrivateAccess = "true"))
	USoundBase* TakePhotoFailSE;

	/// <summary>
	/// 映られた動物
	/// </summary>
	TSet<TWeakObjectPtr<AWildAnimal>> CapturedAnimals;

	/// <summary>
	/// 写真を撮る際に見えないようにするUI
	/// </summary>
	TArray<UWidgetComponent*> NeedCloseUIs;

	/// <summary>
	/// 動物マネージャー
	/// </summary>
	UAnimalManagerComponent* AnimalManager;
};