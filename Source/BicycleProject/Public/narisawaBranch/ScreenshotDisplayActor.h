// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScreenshotDisplayActor.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;
class UTexture2D;
class UMaterialInstanceDynamic;

/**
 * スクリーンショットを3D空間に表示するアクター
 */
UCLASS()
class BICYCLEPROJECT_API AScreenshotDisplayActor : public AActor
{
	GENERATED_BODY()

public:
	AScreenshotDisplayActor();

protected:
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// スクリーンショットテクスチャを設定して表示
	/// </summary>
	/// <param name="Screenshot">表示するテクスチャ</param>
	UFUNCTION(BlueprintCallable, Category = "Screenshot")
	void SetScreenshot(UTexture2D* Screenshot);

	/// <summary>
	/// 文字を設定
	/// </summary>
	/// <param name="Contents">内容</param>
	void SetText(TArray<FName> Contents);

	/// <summary>
	/// プレイヤーの前方に配置
	/// </summary>
	/// <param name="PlayerLocation">プレイヤーの位置</param>
	/// <param name="PlayerForward">プレイヤーの前方向</param>
	/// <param name="Distance">プレイヤーからの距離</param>
	UFUNCTION(BlueprintCallable, Category = "Screenshot")
	void PlaceInFrontOfPlayer(FVector PlayerLocation, FVector PlayerForward, float Distance = 300.0f);

private:

	/// <summary>
	/// 移動(アニメーション)
	/// </summary>
	/// <param name="DeltaTime"></param>
	void AnimMoving(float DeltaTime);

	/// <summary>
	/// 文字を表示するアニメーション
	/// </summary>
	void ShowText(float DeltaTime);

	/// <summary>
	/// 画像を表示するメッシュ
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DisplayMesh;

	/// <summary>
	/// 文字を表示する
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* DisplayText;

	/// <summary>
	/// ベースマテリアル（BPで設定）
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* BaseMaterial;

	/// <summary>
	/// 動的マテリアルインスタンス
	/// </summary>
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	/// <summary>
	/// 表示サイズ（幅）
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	float DisplayWidth;

	/// <summary>
	/// 表示サイズ（高さ）
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	float DisplayHeight;

	/// <summary>
	/// 開始位置(アニメーション用)
	/// </summary>
	FVector StartLocationForAnim;

	/// <summary>
	/// 最終位置(アニメーション用)
	/// </summary>
	FVector EndLocationForAnim;

	/// <summary>
	/// 移動方向(アニメーション用)
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	FVector DeltaLocationForAnim;

	/// <summary>
	/// 移動時間
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	float MovingDuration;

	/// <summary>
	/// 移動タイマー
	/// </summary>
	float AnimMovingTime;

	/// <summary>
	/// 文字の大きさ
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	float TargetSize;

	/// <summary>
	/// 文字のサイズ変更時間
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	float SizeChangeDuration;

	/// <summary>
	/// 文字のサイズ変更タイマー
	/// </summary>
	float SizeChangeTime;
};