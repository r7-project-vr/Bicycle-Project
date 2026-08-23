// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WuBranch/Interface/PauseInterface.h"
#include "WuBranch/Enum/TutorialType.h"
#include "BikeCharacter.generated.h"

class UBikeComponent;
class UBikeMovementComponent;
class UResponderComponent;
class UAnimalManagerComponent;
class UPhotoCaptureComponent;
class UBoxComponent;
class AQuestionUIActor;
class UMyHandRootComponent;
class ATablet;
class UWidgetComponent;

enum class ETabletState : uint8
{
	Highest,
	Up,
	Stay,
	Down,
	Lowest
};

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
	/// 移動停止
	/// </summary>
	void StopMove();

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
	/// オートプレイを起動
	/// </summary>
	/// <param name="Quiz">クイズ</param>
	void EnableAutoPlay(AQuestionUIActor* Quiz);

	/// <summary>
	/// オートプレイをキャンセル
	/// </summary>
	void DisableAutoPlay();

	/// <summary>
	/// チュートリアルを開く
	/// </summary>
	/// <param name="Type">チュートリアルのタイプ</param>
	void OpenTutorial(ETutorialType Type);

	/// <summary>
	/// タブレットを持つ
	/// </summary>
	/// <param name="Hand">手のコリジョン</param>
	/// <returns>成功かどうか、true: 成功, false: 失敗</returns>
	bool TakeTablet(UPrimitiveComponent* Hand);

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
	/// タブレットを生成(チュートリアル用)
	/// </summary>
	void SpawnTablet();

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
	/// 移動のコンポーネントを探す
	/// </summary>
	void FindMover();

	/// <summary>
	/// 回答者を探す
	/// </summary>
	void FindResponder();

	/// <summary>
	/// 撮影者を探す
	/// </summary>
	void FindPhotoCapture();

	/// <summary>
	/// 手を伸ばすものを探す
	/// </summary>
	void FindHandStretcher();

	/// <summary>
	/// タブレット台座のアイコンを探す
	/// </summary>
	void FindTabletBaseIcon();

	UFUNCTION()
	void OnTabletBaseOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/// <summary>
	/// タブレットを上昇させる
	/// </summary>
	/// <param name="DeltaTime"></param>
	void MoveUpTablet(float DeltaTime);

	/// <summary>
	/// タブレットを下げる
	/// </summary>
	/// <param name="DeltaTime"></param>
	void MoveDownTablet(float DeltaTime);

	/// <summary>
	/// チュートリアルをオフ
	/// </summary>
	void CloseTutorial(ATablet* TabletToRetrieve);

	/// <summary>
	/// タブレット台座を表示
	/// </summary>
	void OpenTabletBase();

	/// <summary>
	/// タブレット台座を非表示
	/// </summary>
	void CloseTabletBase();

	/// <summary>
	/// ロード先の自転車メッシュ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<USkeletalMesh> _bikeSkeletalNeedLoad;

	/// <summary>
	/// 自転車の機能
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBikeComponent* Bike;

	/// <summary>
	/// 移動
	/// </summary>
	UBikeMovementComponent* BikeMovement;

	/// <summary>
	/// 回答者
	/// </summary>
	UResponderComponent* Responder;

	/// <summary>
	/// 動物管理者
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UAnimalManagerComponent* AnimalManager;

	/// <summary>
	/// 写真撮影コンポーネント
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UPhotoCaptureComponent* PhotoCapture;

	/// <summary>
	/// 左手のタグ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	FName LeftHandRootTag;

	/// <summary>
	/// 右手のタグ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	FName RightHandRootTag;

	/// <summary>
	/// 手を伸ばすコンポーネント
	/// </summary>
	UMyHandRootComponent* LeftHandRoot;
	UMyHandRootComponent* RightHandRoot;

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
	UPROPERTY(BlueprintReadWrite, Category = "Bike", meta = (AllowPrivateAccess = "true"))
	float HandleBarsAngle;

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

	/// <summary>
	/// タブレットのサンプル
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Tutorial", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ATablet> TabletSample;

	/// <summary>
	/// タブレット
	/// </summary>
	ATablet* Tablet;

	/// <summary>
	/// タブレットの台座
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TabletBase;

	/// <summary>
	/// タブレットのアイコン
	/// </summary>
	UWidgetComponent* TabletBaseIcon;

	/// <summary>
	/// タブレット今の状態
	/// </summary>
	ETabletState TabletCurrentState;

	/// <summary>
	/// タブレットの移動速度
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Tutorial", meta = (AllowPrivateAccess = "true"))
	float TabletMoveVelocity;

	/// <summary>
	/// 移動精度
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Tutorial", meta = (AllowPrivateAccess = "true"))
	float TabletMovementAccuracy;

	/// <summary>
	/// タブレットの最高点
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Tutorial", meta = (AllowPrivateAccess = "true"))
	FVector TabletHighestLocation;

	/// <summary>
	/// タブレットの最低点
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "Tutorial", meta = (AllowPrivateAccess = "true"))
	FVector TabletLowestLocation;
};
