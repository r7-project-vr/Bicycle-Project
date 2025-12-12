// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include <AIController.h>
#include "AnimalDataAsset.generated.h"

UENUM(BlueprintType)
enum class EAnimalType : uint8
{
	Wild,       // 野生
	Pet			// ペット
};

/// <summary>
/// 野生動物の設定
/// </summary>
USTRUCT(BlueprintType)
struct FWildAnimalSettings
{
	GENERATED_BODY()

	/// <summary>
	/// 回転スピード
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RotationSpeed;
};

/// <summary>
/// ペットの設定
/// </summary>
USTRUCT(BlueprintType)
struct FPetAnimalSettings
{
	GENERATED_BODY()

	/// <summary>
	/// ターゲットを追う割合、0～0.99
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TargetChaseRate;

	/// <summary>
	/// 追い始まる距離
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StartChaseDistance;

	/// <summary>
	/// 追わなくなる距離
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GiveUpDistance;
};

/**
 * 
 */
UCLASS()
class BICYCLEPROJECT_API UAnimalDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	/// <summary>
	/// ID
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
	int32 AnimalID = 0;

	/// <summary>
	/// タイプ
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
	EAnimalType AnimalType = EAnimalType::Wild;

	/// <summary>
	/// メッシュ
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	USkeletalMesh* Mesh;

	/// <summary>
	/// メッシュのトランスフォーム
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	FTransform MeshTransform = FTransform();

	/// <summary>
	/// コリジョンサイズ
	/// X: 半径、Y: 半径、Z: 高さ
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	FVector CollisionSize = FVector(34.f, 34.f, 88.f);

	/// <summary>
	/// アニメーション
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TSubclassOf<UAnimInstance> AnimClass;

	/// <summary>
	/// AIコントローラー
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common", meta = (EditCondition = "AnimalType == EAnimalType::Wild"))
	TSubclassOf<AAIController> AIControllerClass;

	/// <summary>
	/// 移動スピード
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
	float MoveSpeed = 200.f;

	/// <summary>
	/// 鳴くサウンド
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
	USoundBase* ChirpSE;

	/// <summary>
	/// 野生動物の設定
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeSettings", meta = (EditCondition = "AnimalType == EAnimalType::Wild"))
	FWildAnimalSettings WildSettings;

	/// <summary>
	/// ペットの設定
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeSettings", meta = (EditCondition = "AnimalType == EAnimalType::Pet"))
	FPetAnimalSettings PetSettings;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("AnimalData", GetFName());
	}
};
