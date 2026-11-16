// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/TimelineComponent.h"
#include "HologramComponent.generated.h"

class UNiagaraComponent;
class UWidgetComponent;
class UTutorialMenuWidget;
class USphereComponent;
class UTutorialIconWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UHologramComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHologramComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// ホログラムを開く
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ActivateHologram();

	/// <summary>
	/// ホログラムを閉じる
	/// </summary>
	void DeactivateHologram();

private:

	enum HologramProcess
	{
		Stop,
		Extending,
		Retracting,
		Expanding,
		Collapsing
	};

	/// <summary>
	/// チュートリアルスクリーンを探す
	/// </summary>
	/// <param name="Childs"></param>
	/// <returns></returns>
	UTutorialMenuWidget* FindScreen(const TArray<USceneComponent*>& Childs);

	/// <summary>
	/// チュートリアルアイコンを探す
	/// </summary>
	/// <param name="Childs"></param>
	/// <returns></returns>
	UTutorialIconWidget* FindIcon(const TArray<USceneComponent*>& Childs);

	/// <summary>
	/// ホログラムの光線を探す
	/// </summary>
	/// <returns></returns>
	UNiagaraComponent* FindHologramLine(const TArray<USceneComponent*>& Childs);

	/// <summary>
	/// チュートリアルスクリーンの初期化
	/// </summary>
	void InitializeTutorialScreen();

	/// <summary>
	/// チュートリアルスクリーンの位置を設定
	/// </summary>
	void DecideTutorialScreenHeight();

	/// <summary>
	/// 展開角度を計算
	/// </summary>
	void CaculateExpandAngle();

	/// <summary>
	/// ホログラム光線を伸ばす
	/// </summary>
	/// <param name="DeltaTime"></param>
	void ExtendHologramLine(float DeltaTime);

	/// <summary>
	/// ホログラム光線を縮む
	/// </summary>
	/// <param name="DeltaTime"></param>
	void RetractHologramLine(float DeltaTime);

	/// <summary>
	/// ホログラム光線を展開
	/// </summary>
	/// <param name="DeltaTime"></param>
	void ExpandHologramLine(float DeltaTime);

	/// <summary>
	/// ホログラム光線を折りたたむ
	/// </summary>
	/// <param name="DeltaTime"></param>
	void CollapseHologramLine(float DeltaTime);

	/// <summary>
	/// ホログラム光線の角度を変更する
	/// </summary>
	void ChangeHologramAngle();

	/// <summary>
	/// チュートリアルスクリーンを見せる
	/// </summary>
	void ShowTutorialScreen();

	/// <summary>
	/// チュートリアルスクリーンが閉じたら
	/// </summary>
	UFUNCTION()
	void OnScreenClose();

	/// <summary>
	/// ホログラムの光線
	/// </summary>
	UPROPERTY()
	UNiagaraComponent* HologramLine;

	/// <summary>
	/// チュートリアルスクリーンのウィジェット
	/// </summary>
	UPROPERTY()
	UWidgetComponent* TutorialWidget;

	/// <summary>
	/// チュートリアルスクリーンのタグ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	FName TutorialTag;

	/// <summary>
	/// チュートリアルスクリーン
	/// </summary>
	UPROPERTY()
	UTutorialMenuWidget* TutorialScreen;

	/// <summary>
	/// チュートリアルアイコンの範囲
	/// </summary>
	UPROPERTY()
	USphereComponent* TutorialIconRange;

	/// <summary>
	/// チュートリアルアイコンのウィジェット
	/// </summary>
	UPROPERTY()
	UWidgetComponent* TutorialIconWidget;

	/// <summary>
	/// チュートリアルアイコン
	/// </summary>
	UPROPERTY()
	UTutorialIconWidget* TutorialIcon;

	/// <summary>
	/// チュートリアルアイコンのタグ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	FName TutorialIconTag;

	/// <summary>
	/// ホログラムの今の状態
	/// </summary>
	HologramProcess CurrentProcess;

	/// <summary>
	/// 経過時間
	/// </summary>
	float ElapsedTime;

	/// <summary>
	/// ホログラム光線の長さ
	/// </summary>
	float HologramLineLength;

	/// <summary>
	/// ホログラム光線今の長さ
	/// </summary>
	float CurrentLength;

	/// <summary>
	/// 伸長にかかる時間
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float ExtendTime;

	/// <summary>
	/// 展開後の角度(単位：度)
	/// </summary>
	//UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float HologramExpandAngle;

	/// <summary>
	/// ホログラム光線今の角度
	/// </summary>
	float CurrentAngle;

	/// <summary>
	/// 基準角度(上方向を基準としているため、基準角度は90度です)
	/// </summary>
	const float BASE_ANGLE = 90.f;

	/// <summary>
	/// 展開にかかる秒数
	/// </summary>
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float ExpandTime;
};
