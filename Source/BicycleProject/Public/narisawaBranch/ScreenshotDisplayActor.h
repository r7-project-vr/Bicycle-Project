// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScreenshotDisplayActor.generated.h"

class UStaticMeshComponent;
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
	/// <summary>
	/// スクリーンショットテクスチャを設定して表示
	/// </summary>
	/// <param name="Screenshot">表示するテクスチャ</param>
	UFUNCTION(BlueprintCallable, Category = "Screenshot")
	void SetScreenshot(UTexture2D* Screenshot);

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
	/// 画像を表示するメッシュ
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DisplayMesh;

	/// <summary>
	/// ベースマテリアル（BPで設定）
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screenshot", meta = (AllowPrivateAccess = "true"))
	UMaterial* BaseMaterial;

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
};