// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WuBranch/Enum/TutorialType.h"
#include "Tablet.generated.h"

class UBoxComponent;
class USpotLightComponent;

UCLASS()
class BICYCLEPROJECT_API ATablet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATablet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// チュートリアルを表示する
	/// </summary>
	/// <param name="Type">チュートリアルのタイプ</param>
	void ShowTutorial(ETutorialType Type);

	/// <summary>
	/// 基準点を設置
	/// </summary>
	/// <param name="Point">基準点</param>
	void SetReferencePoint(FVector Point);

	/// <summary>
	/// 隠す高さを設定
	/// </summary>
	/// <param name="Height"></param>
	void SetHideHeight(float Height);

	/// <summary>
	/// 画面を閉じる
	/// </summary>
	void CloseScene();

	/// <summary>
	/// 画面を表示
	/// </summary>
	void OpenScene();

	/// <summary>
	/// タブレットを隠す
	/// </summary>
	void Hide();

	/// <summary>
	/// タブレットを表示
	/// </summary>
	void Show();

	/// <summary>
	/// コリジョンを起動
	/// </summary>
	void EnableCollision();

	/// <summary>
	/// コリジョンをオフ
	/// </summary>
	void DisableCollision();

	/// <summary>
	/// 通知フラッシュ
	/// </summary>
	void FlashForNotify();

	/// <summary>
	/// 画面を回転する(持つ手を替わった)
	/// </summary>
	/// <param name="bIsLeftHand">左手で持つか</param>
	void RotateScene(bool bIsLeftHand);

	/// <summary>
	/// 通知を止める
	/// </summary>
	void StopNotify();

protected:

	UFUNCTION(BlueprintCallable)
	void NextTutorial();

	UFUNCTION(BlueprintCallable)
	void PreTutorial();

private:

	UFUNCTION()
	void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/// <summary>
	/// チュートリアルの画像を変更
	/// </summary>
	/// <param name="Target">画像</param>
	void ChangeTexture(UTexture2D* Texture);

	/// <summary>
	/// フラッシュ一回
	/// </summary>
	/// <param name="Sec">光の継続秒数</param>
	void FlashOneTime(float Sec);

	/// <summary>
	/// 光る強さを変更
	/// </summary>
	/// <param name="Intensity">光る強さ</param>
	void ChangeLightIntensity(float Intensity);

	/// <summary>
	/// メッシュ
	/// </summary>
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TabletMesh;

	/// <summary>
	/// コリジョン
	/// </summary>
	UPROPERTY(EditAnywhere)
	UBoxComponent* TabletCollision;

	/// <summary>
	/// 表示する内容
	/// </summary>
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Content;

	/// <summary>
	/// 使用しているチュートリアル
	/// </summary>
	TArray<UTexture2D*> CurrentTutorial;

	/// <summary>
	/// クイズのチュートリアルのテクスチャー
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial", meta = (AllowPrivateAccess = "true"))
	TArray<UTexture2D*> QuizTutorials;

	/// <summary>
	/// スピードオーバーのチュートリアルのテクスチャー
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial", meta = (AllowPrivateAccess = "true"))
	TArray<UTexture2D*> SpeedOverTutorials;

	/// <summary>
	/// コインのチュートリアルのテクスチャー
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial", meta = (AllowPrivateAccess = "true"))
	TArray<UTexture2D*> CoinTutorials;

	/// <summary>
	/// 写真のチュートリアルのテクスチャー
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial", meta = (AllowPrivateAccess = "true"))
	TArray<UTexture2D*> PhotoTutorials;

	/// <summary>
	/// チュートリアルのインデックス
	/// </summary>
	int TextureIndex;

	/// <summary>
	/// タブレットのライト
	/// </summary>
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Light;

	/// <summary>
	/// フラッシュのマテリアル
	/// </summary>
	UMaterialInstanceDynamic* FlashDynamicMaterial;

	/// <summary>
	/// フラッシュのタイマー
	/// </summary>
	FTimerHandle FlashTimer;

	/// <summary>
	/// 通知のタイマー
	/// </summary>
	FTimerHandle NotifyTimer;

	/// <summary>
	/// 通知音
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tutorial", meta = (AllowPrivateAccess = "true"))
	USoundBase* NotifySE;

	/// <summary>
	/// 基準点
	/// </summary>
	FVector ReferencePoint;
};
