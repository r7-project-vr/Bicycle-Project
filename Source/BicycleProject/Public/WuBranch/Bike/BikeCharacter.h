// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WuBranch/Interface/PauseInterface.h"
#include "BikeCharacter.generated.h"

class UBikeComponent;
class UAnimalManagerComponent;

UCLASS()
class BICYCLEPROJECT_API ABikeCharacter : public ACharacter, public IPauseInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABikeCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Pause_Implementation() override;
	void ReStart_Implementation() override;
	bool IsPause_Implementation() override;

	/// <summary>
	/// バイクのメッシュを変更
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ChangeBikeMesh();

	/// <summary>
	/// スピードを取得
	/// </summary>
	/// <returns>スピード</returns>
	UFUNCTION(BlueprintCallable)
	float GetSpeed();

	/// <summary>
	/// ハンドラーの角度を取得
	/// </summary>
	/// <returns>ハンドラーの角度</returns>
	UFUNCTION(BlueprintCallable)
	float GetHandlerAngle() const;

	/// <summary>
	/// 曲がるための角度を設置
	/// </summary>
	/// <param name="angle">角度</param>
	void SetTurningAngle(FRotator angle);

	/// <summary>
	/// UI補助線を表示しない
	/// </summary>
	void DisableHintLine();

	/// <summary>
	/// BikeComponentをゲット
	/// </summary>
	/// <returns></returns>
	UBikeComponent* GetBikeComponent();

	/// <summary>
	/// 超速したか
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	bool HasOverSpeed() const;

	/// <summary>
	/// 超速の状態をリセット
	/// </summary>
	void ResetOverSpeed();

	/// <summary>
	/// スクリーンショット撮影後に呼ばれる
	/// </summary>
	UFUNCTION()
	void OnScreenshotTaken();

private:

	/// <summary>
	/// 自転車メッシュをロード
	/// </summary>
	void LoadBikeMesh();

	/// <summary>
	/// ロード終わったら
	/// </summary>
	void LoadMeshComplete();

	/// <summary>
	/// 自転車の方向を変える
	/// </summary>
	void RotateBike(float DeltaTime);

	/// <summary>
	/// 速すぎたかのチェック
	/// </summary>
	/// <returns>true: はい, false: いいえ</returns>
	bool CheckOverSpeed() const;

	/// <summary>
	/// ロード先の自転車メッシュ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<USkeletalMesh> _bikeSkeletalNeedLoad;

	/// <summary>
	/// 自転車の機能
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBikeComponent* _bike;

	/// <summary>
	/// 動物管理者
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UAnimalManagerComponent* AnimalManager;

	/// <summary>
	/// UI操作用のヒントライン
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UWidgetInteractionHeadComponent* _widgetInteractionHeadComponent;

	/// <summary>
	/// 曲がるか
	/// </summary>
	bool _isRotate;

	/// <summary>
	/// 曲がるスピード
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	float _rotateSpeed = 10.f;

	/// <summary>
	/// 曲がる時の最終角度
	/// </summary>
	FRotator _targetRotator;

	/// <summary>
	/// ハンドルの角度
	/// </summary>
	float _handlebarsAngle;

	/// <summary>
	/// ハンドルの戻り速度
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	float _handlebarCenteringSpeed;

	/// <summary>
	/// 超速したか
	/// </summary>
	bool IsOverSpeed;

	/// <summary>
	/// 停止中ですか
	/// </summary>
	bool IsPause;
};
