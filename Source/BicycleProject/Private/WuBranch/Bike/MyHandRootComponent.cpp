// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/MyHandRootComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MotionControllerComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include <WuBranch/UI/QuestionUIActor.h>
#include "Components/WidgetInteractionComponent.h"
#include "Components/WidgetComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Device/DeviceManager.h"

// Sets default values for this component's properties
UMyHandRootComponent::UMyHandRootComponent()
	: ControllerTag("")
	, Owner(nullptr)
	, VRController(nullptr)
	, HandMesh(nullptr)
	, HandCollision(nullptr)
	, bIsControlled(false)
	, StretchInitialVelocity(100.f)
	, StretchVelocityDamping(20.f)
	, CurrentVelocity(0.f)
	, OriginalVRLocation(FVector::ZeroVector)
	, OriginalHandLocation(FVector::ZeroVector)
	, ExtensionAngle(FRotator(0.f, -90.f, 0.f))
	, StretchStartLocation(FVector::ZeroVector)
	, Direction(FVector::ZeroVector)
	, bIsStretching(false)
	, Sensitivity(3.f)
	, bIsPlayingRetractSE(false)
	, HandRotateAcceleration(3.f)
	, HandRotateMaxVelocity(30.f)
	, HandRotateVelocity(30.f)
	, HandRotateRadius(50.f)
	, HandRotateAngle(0.f)
	, bIsRotateArm(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UMyHandRootComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	FindOwner();
	FindVRControllerByTag();
	FindHandMesh();
	FindDeviceManager();

	if (HandMesh)
	{
		OriginalHandLocation = HandMesh->GetRelativeLocation();
		OriginalRotation = HandMesh->GetRelativeRotation();
		
		FindHandCollisionAndInteractor();
		if (HandCollision)
		{
			// WidgetInteractionComponentを使ってUIに触ったかどうかを判断
			// コリジョンのOverlapを使わない理由は、時々UIをすり抜けるからです。
			HandWidgetInteractor->OnHoveredWidgetChanged.AddDynamic(this, &ThisClass::OnWidgetChanged);
		}
	}
}

// Called every frame
void UMyHandRootComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!bIsControlled && VRController)
	{
		SetRelativeLocation(VRController->GetRelativeLocation());
		SetRelativeRotation(VRController->GetRelativeRotation());
	}
	else if (bIsRotateArm)
	{
		RotateArm(DeltaTime);
	}
	else if (bIsStretching)
	{
		Stretch(DeltaTime);
	}
}

void UMyHandRootComponent::OnStretchArm(bool bIsPrepared)
{
	if (!HandMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Hand Mesh is null!"));
		return;
	}

	// 手が伸ばしている途中なら無視、
	if (bIsStretching)
		return;

	if (bIsPrepared)
	{
		// 手を伸ばす前の準備段階
		// 手の角度を固定する(欲しい軌道に沿って回転させるため)
		SetRelativeRotation(HandRotationAngle);
		RecordInitialInfo();

		// MotionControllerに同期しない
		bIsControlled = true;

		// 手を回転させる準備
		PrepareRotateArm();
	}
	else if (bIsRotateArm)
	{
		if (!Owner)
			return;

		// 回すアニメーションを停止
		StopRotateSE();
		bIsRotateArm = false;

		// 手を伸ばす
		// 伸ばす方向を計算(コントローラーの位置を利用する)
		FVector EndLocation = VRController->GetRelativeLocation();
		Direction = EndLocation - OriginalVRLocation;
		// 方向のYとZ軸の感度を下げる
		Direction.Y /= Sensitivity;
		Direction.Z /= Sensitivity;
		Direction = Direction.GetSafeNormal();
		// 移動方向をキャラクターの向きに合わせる
		Direction = Owner->GetActorRotation().RotateVector(Direction);
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Orange, FString::Printf(TEXT("Dir: %s"), *Direction.ToString()));
		// 固定の位置から手を伸ばす
		HandMesh->SetRelativeLocation(FVector::ZeroVector);
		// 伸ばす初速を設定
		CurrentVelocity = StretchInitialVelocity;
		PlayStretchSE();
		StretchLength = 0.f;
		PreviousStretchLength = 0.f;
		bIsStretching = true;
	}
}

void UMyHandRootComponent::ForceReset()
{
	if (bIsRotateArm)
	{
		bIsRotateArm = false;
		HandRotateVelocity = 0;
		ReturnToOriginalTransform();
		bIsControlled = false;
	}
	else if(bIsStretching)
		ResetHand();
}

void UMyHandRootComponent::FindOwner()
{
	if (AActor* Character = GetOwner())
	{
		Owner = Character;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Didnot find Owner!!"));
	}
}

void UMyHandRootComponent::FindVRControllerByTag()
{
	if (Owner)
	{
		VRController = Owner->FindComponentByTag<UMotionControllerComponent>(ControllerTag);

		if (!VRController)
		{
			UE_LOG(LogTemp, Error, TEXT("Didnot find Motion Controller, check tag!!"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Didnot have owner in FindVRControllerByTag!")));
	}
}

void UMyHandRootComponent::FindHandMesh()
{
	TArray<USceneComponent*> ChildComponents;
	GetChildrenComponents(true, ChildComponents);
	for (auto Child : ChildComponents)
	{
		if (USkeletalMeshComponent* Hand = Cast<USkeletalMeshComponent>(Child))
		{
			HandMesh = Hand;
		}
	}

	if (ChildComponents.Num() <= 0 || !HandMesh)
		UE_LOG(LogTemp, Error, TEXT("Didnot find Hand Skeletal Mesh Component under hand root!"));
}

void UMyHandRootComponent::FindHandCollisionAndInteractor()
{
	TArray<USceneComponent*> ChildComponents;
	HandMesh->GetChildrenComponents(true, ChildComponents);
	for (auto Child : ChildComponents)
	{
		if (USphereComponent* Collision = Cast<USphereComponent>(Child))
		{
			HandCollision = Collision;
		}

		if (UWidgetInteractionComponent* Interactor = Cast<UWidgetInteractionComponent>(Child))
		{
			HandWidgetInteractor = Interactor;
		}
	}
}

void UMyHandRootComponent::FindDeviceManager()
{
	if (UWorld* World = GetWorld())
	{
		UMyGameInstance* MyInstance = Cast<UMyGameInstance>(World->GetGameInstance());
		if (MyInstance)
		{
			DeviceManager = MyInstance->GetDeviceManager();
		}
	}
}

void UMyHandRootComponent::OnWidgetChanged(UWidgetComponent* WidgetComponent, UWidgetComponent* PreviousWidgetComponent)
{
	if (WidgetComponent)
	{
		// 今伸ばしているなら
		if (bIsStretching)
		{
			CurrentVelocity = 0.f;
			FHitResult Result = HandWidgetInteractor->GetLastHitResult();
			// サウンド再生
			PlayTouchedSE(Result.ImpactPoint);
			// エフェクト
			PlayTouchEffect(Result);
		}
	}
}

void UMyHandRootComponent::PlayTouchEffect(const FHitResult& HitResult)
{
	if (!TouchEffect)
		return;

	// Overlapから貰ったFHitResultを使わない理由は、GetActorでの値はあるが、ImapctPointの値はずっと(0,0,0)です。
	// なので、HandWidgetInteractorからHitResultを貰うにした
	if (!HitResult.GetActor())
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, TEXT("Didnot Touch"));
		return;
	}
	FVector FaceTo = HitResult.GetActor()->GetActorForwardVector();
	// エフェクトの一部が選択肢に覆われているため、ちょっと手前に移動する
	FVector Location = HitResult.ImpactPoint + FaceTo * 5;
	FRotator Rotation = HitResult.GetActor()->GetActorRotation();

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TouchEffect, Location, Rotation, FVector::OneVector, false);
}

void UMyHandRootComponent::PrepareRotateArm()
{
	// 手の位置と角度を固定
	//SetRelativeLocation(FVector::ZeroVector);FVector(HandRotateRadius, 0.f, 0.f)
	HandMesh->SetRelativeLocation(FVector::ZeroVector);
	HandMesh->SetRelativeRotation(ExtensionAngle);
	HandRotateAngle = 0;
	HandRotateVelocity = 0;
	PlayRotateSE();
	// 前進機能を止める
	
	bIsRotateArm = true;
}

void UMyHandRootComponent::RotateArm(float DeltaTime)
{
	// 回転は遅めから段々と速くなる、最大値を超えたら速度を維持する
	if (HandRotateVelocity >= HandRotateMaxVelocity)
		HandRotateVelocity = HandRotateMaxVelocity;
	else
		HandRotateVelocity += HandRotateAcceleration * DeltaTime;

	// 回転速度を角度に反映する
	HandRotateAngle -= HandRotateVelocity * DeltaTime;

	float X = FMath::Cos(HandRotateAngle) * HandRotateRadius;
	float Z = FMath::Sin(HandRotateAngle) * HandRotateRadius;

	HandMesh->SetRelativeLocation(FVector(X, 0, Z));
}

void UMyHandRootComponent::RecordInitialInfo()
{
	// コントローラーの座標を記録
	OriginalVRLocation = VRController->GetRelativeLocation();

	// 今手の座標を記録
	StretchStartLocation = HandMesh->GetComponentLocation();
	StretchingLocation = HandMesh->GetComponentLocation();
}

void UMyHandRootComponent::Stretch(float DeltaTime)
{
	FVector Distance = Direction * CurrentVelocity * DeltaTime;

	// 世界座標を使う、事前にDirectionをキャラクターの向きに合わせる必要がある
	// 相対位置を使うと、プレイヤーの手の向きにより想定していない方向に移動してしまう問題がある
	StretchingLocation += Distance;
	HandMesh->SetWorldLocation(StretchingLocation);
	//FVector NewHandLocation = HandMesh->GetComponentLocation() + Distance;
	//HandMesh->SetWorldLocation(NewHandLocation);

	// 次回の速度
	CurrentVelocity -= StretchVelocityDamping * DeltaTime;

	// 引っ込め始めた
	if (!bIsPlayingRetractSE && CurrentVelocity <= 0)
	{
		StopStretchSE();
		PlayRetractSE();
		bIsPlayingRetractSE = true;
	}

	// 速度がマイナスになり、伸ばした手の相対位置がZeroVectorに近い位置になると
	// 手が戻ったと意味する。bIsStretcgingをfalseにする
	PreviousStretchLength = StretchLength;
	StretchLength = (StretchingLocation - StretchStartLocation).Length();
	if (CurrentVelocity <= 0 && StretchLength < 50)
	{
		StopRetractSE();
		ResetHand();
	}
}

void UMyHandRootComponent::ReturnToOriginalTransform()
{
	HandMesh->SetRelativeLocation(OriginalHandLocation);
	HandMesh->SetRelativeRotation(OriginalRotation);
}

void UMyHandRootComponent::EnableMoveAction()
{
	if (!DeviceManager)
		FindDeviceManager();

	if(DeviceManager)
		DeviceManager->EnableDefaultActions();
}

void UMyHandRootComponent::DisableMoveAction()
{
	if (!DeviceManager)
		FindDeviceManager();

	if (DeviceManager)
		DeviceManager->DisableDefaultActions();
}

void UMyHandRootComponent::ResetHand()
{
	bIsStretching = false;
	CurrentVelocity = 0.f;
	Direction = FVector::ZeroVector;
	ReturnToOriginalTransform();
	bIsPlayingRetractSE = false;
	bIsControlled = false;
}
