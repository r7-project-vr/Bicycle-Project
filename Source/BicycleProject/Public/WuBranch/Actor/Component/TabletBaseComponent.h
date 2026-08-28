// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TabletBaseComponent.generated.h"

class USphereComponent;
class UWidgetComponent;
class ATablet;

enum class ETabletState : uint8
{
	Highest,
	Up,
	Stay,
	Down,
	Lowest
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UTabletBaseComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTabletBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/// <summary>
	/// 台座を表示
	/// </summary>
	void Open();

	/// <summary>
	/// 台座を非表示
	/// </summary>
	void Close();

	/// <summary>
	/// 強制的にタブレットを回収
	/// </summary>
	void ForceRecycleTablet();

	/// <summary>
	/// タブレットを呼ぶ
	/// </summary>
	void StartUseTablet();

private:

	/// <summary>
	/// 判定範囲を探す
	/// </summary>
	/// <param name="Target">判定範囲の参照</param>
	/// <param name="TargetTag">判定範囲のタグ</param>
	USphereComponent* FindRange(const FName& TargetTag);

	/// <summary>
	/// アイコンウィジェットを探す
	/// </summary>
	void FindIconWidget();

	/// <summary>
	/// 回収範囲に入った
	/// </summary>
	UFUNCTION()
	void OnRecycleRangeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/// <summary>
	/// 台座範囲を出だ
	/// </summary>
	UFUNCTION()
	void OnOutsideBaseRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/// <summary>
	/// タブレットを生成
	/// </summary>
	void SpawnTablet();

	/// <summary>
	/// タブレットの位置を設定
	/// </summary>
	void SetTabletLocation(ETabletState State);

	/// <summary>
	/// 回収範囲を機能させる
	/// </summary>
	void EnableRecycleRange();

	/// <summary>
	/// 回収範囲を機能させない
	/// </summary>
	void DisableRecycleRange();

	/// <summary>
	/// 台座範囲を機能させる
	/// </summary>
	void EnableBaseRange();

	/// <summary>
	/// 台座範囲を機能させない
	/// </summary>
	void DisableBaseRange();

	/// <summary>
	/// 回収アイコンを表示
	/// </summary>
	void ShowRecycleIcon();

	/// <summary>
	/// 回収アイコンを非表示
	/// </summary>
	void HideRecycleIcon();

	/// <summary>
	/// タブレットを移動指せる
	/// </summary>
	/// <param name="DeltaTime"></param>
	void OnMoveTablet(float DeltaTime);

	/// <summary>
	/// 回収範囲（台座範囲より小さい）
	/// </summary>
	USphereComponent* RecycleRange;

	/// <summary>
	/// 回収範囲のタグ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName RecycleRangeTag;

	/// <summary>
	/// 台座範囲
	/// </summary>
	USphereComponent* BaseRange;

	/// <summary>
	/// 台座範囲のタグ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName BaseRangeTag;

	/// <summary>
	/// 回収アイコン
	/// </summary>
	UWidgetComponent* RecycleIcon;

	/// <summary>
	/// 台座範囲のタグ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName RecycleIconTag;

	/// <summary>
	/// タブレットのサンプル
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Tablet", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ATablet> TabletSample;

	/// <summary>
	/// タブレット
	/// </summary>
	ATablet* Tablet;

	/// <summary>
	/// タブレット今の状態
	/// </summary>
	ETabletState TabletCurrentState;

	/// <summary>
	/// タブレットの最高点
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Tablet", meta = (AllowPrivateAccess = "true"))
	FVector TabletHighestLocation;

	/// <summary>
	/// タブレットの最低点
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Tablet", meta = (AllowPrivateAccess = "true"))
	FVector TabletLowestLocation;

	/// <summary>
	/// タブレットの移動速度
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Tablet", meta = (AllowPrivateAccess = "true"))
	float TabletMoveVelocity;

	/// <summary>
	/// 移動精度
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Tablet", meta = (AllowPrivateAccess = "true"))
	float TabletMovementAccuracy;
};
