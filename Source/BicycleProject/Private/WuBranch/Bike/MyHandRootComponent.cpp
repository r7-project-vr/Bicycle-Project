// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/MyHandRootComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MotionControllerComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include <WuBranch/UI/QuestionUIActor.h>
#include "Components/WidgetInteractionComponent.h"
#include "Components/WidgetComponent.h"

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
	, HandLocationTmp(FVector::ZeroVector)
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

	if (HandMesh)
	{
		OriginalHandLocation = HandMesh->GetRelativeLocation();
		OriginalRotation = HandMesh->GetRelativeRotation();
		
		FindHandCollisionAndInteractor();
		if (HandCollision)
		{
			// 手のコリジョンをバインド
			HandCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
			//HandCollision->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
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
	else
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
		bIsStretching = true;
	}
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

void UMyHandRootComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// コリジョンに設定された反応できるオブジェクトの何かに触れた
	if (OtherActor)
	{
		// 今伸ばしているなら
		if (bIsStretching)
		{
			CurrentVelocity = 0.f;
			PlayTouchedSE(SweepResult.ImpactPoint);
		}
			
		/*if (AQuestionUIActor* Question = Cast<AQuestionUIActor>(OtherActor))
		{
			if (HandWidgetInteractor)
			{
				HandWidgetInteractor->PressPointerKey(EKeys::LeftMouseButton);
				if (UWidgetComponent* Widget = HandWidgetInteractor->GetHoveredWidgetComponent())
				{
					FName Name = Widget->GetFName();
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("Press key to %s"), *Name.ToString()));
				}
			}
		}*/
	}
}

void UMyHandRootComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (AQuestionUIActor* Question = Cast<AQuestionUIActor>(OtherActor))
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("Release key"));
			if (HandWidgetInteractor)
				HandWidgetInteractor->ReleasePointerKey(EKeys::LeftMouseButton);
		}
	}
}

void UMyHandRootComponent::PrepareRotateArm()
{
	// 手の位置と角度を固定
	//SetRelativeLocation(FVector::ZeroVector);
	HandMesh->SetRelativeLocation(FVector(HandRotateRadius, 0.f, 0.f));
	HandMesh->SetRelativeRotation(ExtensionAngle);
	HandRotateAngle = 0;
	HandRotateVelocity = 0;
	PlayRotateSE();
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
	HandLocationTmp = HandMesh->GetComponentLocation();
}

void UMyHandRootComponent::Stretch(float DeltaTime)
{
	FVector Distance = Direction * CurrentVelocity * DeltaTime;

	// 世界座標を使う、事前にDirectionをキャラクターの向きに合わせる必要がある
	// 相対位置を使うと、プレイヤーの手の向きにより想定していない方向に移動してしまう問題がある
	FVector NewHandLocation = HandMesh->GetComponentLocation() + Distance;
	HandMesh->SetWorldLocation(NewHandLocation);

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
	if (CurrentVelocity <= 0 && (NewHandLocation - HandLocationTmp).Length() < 50)
	{
		bIsStretching = false;
		bIsControlled = false;
		CurrentVelocity = 0.f;
		Direction = FVector::ZeroVector;
		ReturnToOriginalTransform();
		StopRetractSE();
		bIsPlayingRetractSE = false;
	}
}

void UMyHandRootComponent::ReturnToOriginalTransform()
{
	HandMesh->SetRelativeLocation(OriginalHandLocation);
	HandMesh->SetRelativeRotation(OriginalRotation);
}
