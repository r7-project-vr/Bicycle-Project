// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MyHandRootComponent.generated.h"

class AActor;
class USkeletalMeshComponent;
class UMotionControllerComponent;
class USphereComponent;
class UWidgetInteractionComponent;
class UNiagaraSystem;
class UWidgetComponent;
class UDeviceManager;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UMyHandRootComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyHandRootComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// 手を伸ばす
	/// </summary>
	/// <param name="bIsPrepared"></param>
	UFUNCTION()
	void OnStretchArm(bool bIsPrepared);

protected:

	/// <summary>
	/// 手が伸びている時何かにぶつかったSEを再生
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void PlayTouchedSE(FVector AudioLocation);

	/// <summary>
	/// 手が回る時のSEを再生
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void PlayRotateSE();

	/// <summary>
	/// 手が回る時のSEを停止
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void StopRotateSE();

	/// <summary>
	/// 手を伸ばしている時のSEを再生
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void PlayStretchSE();

	/// <summary>
	/// 手を伸ばしている時のSEを停止
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void StopStretchSE();

	/// <summary>
	/// 手を引っ込める時のSEを再生
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void PlayRetractSE();

	/// <summary>
	/// 手を引っ込める時のSEを停止
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void StopRetractSE();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* TouchEffect;

private:

	/// <summary>
	/// 所有者を探す
	/// </summary>
	void FindOwner();

	/// <summary>
	/// 対象VRコントローラーを探す
	/// </summary>
	void FindVRControllerByTag();

	/// <summary>
	/// 手を探す
	/// </summary>
	void FindHandMesh();

	/// <summary>
	/// 手のコリジョンを探す
	/// </summary>
	void FindHandCollisionAndInteractor();

	/// <summary>
	/// デバイスマネジャーを探す
	/// </summary>
	void FindDeviceManager();

	/// <summary>
	/// 手がUIに触った処理
	/// </summary>
	UFUNCTION()
	void OnWidgetChanged(UWidgetComponent* WidgetComponent, UWidgetComponent* PreviousWidgetComponent);

	/// <summary>
	/// 手がUIに触ったエフェクトを再生
	/// </summary>
	void PlayTouchEffect(const FHitResult& HitResult);

	/// <summary>
	/// 腕を回転させ前の準備段階
	/// </summary>
	void PrepareRotateArm();

	/// <summary>
	/// 腕を回転させる
	/// </summary>
	void RotateArm(float DeltaTime);

	/// <summary>
	/// 手を伸ばす前のデータを記録する
	/// </summary>
	void RecordInitialInfo();

	/// <summary>
	/// 手を伸ばす
	/// </summary>
	void Stretch(float DeltaTime);

	/// <summary>
	/// 元の位置と角度に戻る
	/// </summary>
	void ReturnToOriginalTransform();

	/// <summary>
	/// 移動機能をオンにする
	/// </summary>
	void EnableMoveAction();

	/// <summary>
	/// 移動機能をオフにする
	/// </summary>
	void DisableMoveAction();

	/// <summary>
	/// VRコントローラーのタグ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	FName ControllerTag;

	/// <summary>
	/// 所有者
	/// </summary>
	AActor* Owner;

	/// <summary>
	/// VRコントローラー
	/// </summary>
	UMotionControllerComponent* VRController;

	/// <summary>
	/// 手
	/// </summary>
	USkeletalMeshComponent* HandMesh;

	/// <summary>
	/// 手のコリジョン
	/// </summary>
	USphereComponent* HandCollision;

	/// <summary>
	/// VR世界のUIを反応させるコンポーネント
	/// </summary>
	UWidgetInteractionComponent* HandWidgetInteractor;

	/// <summary>
	/// デバイスマネジャー
	/// </summary>
	UDeviceManager* DeviceManager;

	/// <summary>
	/// 制御されているか
	/// </summary>
	bool bIsControlled;

	/// <summary>
	/// 伸ばす際の初速
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float StretchInitialVelocity;

	/// <summary>
	/// 伸ばす際の減衰
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float StretchVelocityDamping;

	/// <summary>
	/// 今の速度
	/// </summary>
	float CurrentVelocity;

	/// <summary>
	/// VRコントローラー本来の位置
	/// </summary>
	FVector OriginalVRLocation;

	/// <summary>
	/// 元の手の相対位置
	/// </summary>
	FVector OriginalHandLocation;

	/// <summary>
	/// 今の手の世界座標を記録
	/// </summary>
	FVector HandLocationTmp;

	/// <summary>
	/// 元の手の相対角度
	/// </summary>
	FRotator OriginalRotation;

	/// <summary>
	/// 伸ばすときの手の角度
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	FRotator ExtensionAngle;

	/// <summary>
	/// 伸ばす方向
	/// </summary>
	FVector Direction;

	/// <summary>
	/// 伸ばし中ですか
	/// </summary>
	bool bIsStretching;

	/// <summary>
	/// 手を伸ばす時の角度の感度
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float Sensitivity;

	/// <summary>
	/// 手を引っ込める時のSEが再生中ですか
	/// </summary>
	bool bIsPlayingRetractSE;

	/// <summary>
	/// 腕を回転する前の角度
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	FRotator HandRotationAngle;

	/// <summary>
	/// 腕の回転加速度
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float HandRotateAcceleration;

	/// <summary>
	/// 腕の最大回転速度
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float HandRotateMaxVelocity;

	/// <summary>
	/// 腕の回転速度
	/// </summary>
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float HandRotateVelocity;

	/// <summary>
	/// 腕の回転半径
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float HandRotateRadius;

	/// <summary>
	/// 腕の回転角度
	/// </summary>
	float HandRotateAngle;

	/// <summary>
	/// 腕を回転させるか
	/// </summary>
	bool bIsRotateArm;
};
