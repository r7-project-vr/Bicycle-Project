// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Bike/WidgetInteractionHeadComponent.h"
#include "Components/WidgetComponent.h"
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplay.h"
#include <WuBranch/UI/QuestionUIActor.h>
#include <WuBranch/QuestionGameMode.h>

UWidgetInteractionHeadComponent::UWidgetInteractionHeadComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

	// 基本設定
	TraceChannel = ECC_GameTraceChannel1;
	InteractionDistance = 2000.0f;

	//Line = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Line"));
	//Line->SetupAttachment(this);

	CursorWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CursorWidget"));
	CursorWidget->SetupAttachment(this);
	CursorWidget->SetWidgetSpace(EWidgetSpace::World);
	CursorWidget->SetDrawSize(FVector2D(CursorSize, CursorSize));
	CursorWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CursorWidget->SetCollisionResponseToAllChannels(ECR_Ignore);
	CursorWidget->SetHiddenInGame(true);

	bHintEnabled = false;
}

void UWidgetInteractionHeadComponent::BeginPlay()
{
	Super::BeginPlay();

	OnHoveredWidgetChanged.AddDynamic(this, &UWidgetInteractionHeadComponent::OnHoverWidget);
	DisableHintLine();
}

void UWidgetInteractionHeadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateCursorTransform();
}

void UWidgetInteractionHeadComponent::EnableHintLine()
{
	bHintEnabled = true;
	//Line->SetHiddenInGame(false);
}

void UWidgetInteractionHeadComponent::DisableHintLine()
{
	bHintEnabled = false;
	CursorWidget->SetHiddenInGame(true);
	//Line->SetHiddenInGame(true);
}

void UWidgetInteractionHeadComponent::OnHoverWidget(UWidgetComponent* WidgetComponent, UWidgetComponent* PreviousWidgetComponent)
{
	// ゲーム終了した場合何もしない
	AQuestionGameMode* QuestionGameMode = Cast<AQuestionGameMode>(GetWorld()->GetAuthGameMode());
	// 2025.6.27 start tokumaru questionGameModeのnull参照対策
	if (QuestionGameMode) {
	
		if (QuestionGameMode->IsGameFailed() || QuestionGameMode->IsGameClear())
			return;
	}
	// 2025.6.27 end 
	
	// 前に隠しているウィジェットがあれば、表示させる
	if(WidgetComponent)
	{
		//未回答の問題だけ表示する
		AQuestionUIActor* Question = Cast<AQuestionUIActor>(WidgetComponent->GetOwner());
		// 2025.6.27 start tokumaru questionGameModeのnull参照対策
		if (Question) {
			if (!Question->GetAnsweredStatus())
			{
				Question->DisplayUI();
				EnableHintLine();
			}
		}
		// 2025.6.27 end 
	}
}

bool UWidgetInteractionHeadComponent::IsVRConnect() const
{
	return GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->XRSystem->GetHMDDevice()->IsHMDConnected();
}

void UWidgetInteractionHeadComponent::UpdateCursorTransform()
{
	//　表示するか
	if (!bHintEnabled)
	{
		CursorWidget->SetHiddenInGame(true);
		return;
	}

	//FVector RayOrigin = GetComponentLocation();
	//FVector RayEnd = RayOrigin + GetForwardVector() * InteractionDistance;
	//
	//// LineTraceSingleByChannelを使うため、QuestionUIActorのwidgetにQuestionUIに対する反応はBlockにする必要がある
	//// ovelapにしていると、LineTraceSingleByChannelは反応しないため
	//FHitResult HitResult;
	//FCollisionQueryParams Params;
	//Params.AddIgnoredActor(GetOwner());
	//
	//bool bHit = GetWorld()->LineTraceSingleByChannel(
	//	HitResult,
	//	RayOrigin,
	//	RayEnd,
	//	TraceChannel,
	//	Params
	//);

	//if (!bHit || !HitResult.GetActor())
	//{
	//	CursorWidget->SetHiddenInGame(true);
	//	return;
	//}
	//
	//UWidgetComponent* HitWidget = Cast<UWidgetComponent>(HitResult.GetComponent());
	//if (!HitWidget)
	//{
	//	CursorWidget->SetHiddenInGame(true);
	//	return;
	//}

	//FVector WidgetForward = HitWidget->GetForwardVector();
	//FVector ToPlayer = (RayOrigin - HitResult.ImpactPoint).GetSafeNormal();
	//float Dot = FVector::DotProduct(WidgetForward, ToPlayer);
	//FVector OffsetDirection = (Dot >= 0.0f) ? WidgetForward : -WidgetForward;

	//FVector CursorPosition = HitResult.ImpactPoint + OffsetDirection * CursorOffsetFromUI;
	//FRotator CursorRotation = OffsetDirection.Rotation();

	// レイと平面の交点の計算式を利用する
	UWidgetComponent* HoveredWidget = GetHoveredWidgetComponent();
	if (!HoveredWidget)
	{
		CursorWidget->SetHiddenInGame(true);
		return;
	}

	FVector RayOrigin = GetComponentLocation();
	FVector RayDir = GetForwardVector();

	FVector WidgetForward = HoveredWidget->GetForwardVector();
	FVector WidgetCenter = HoveredWidget->GetComponentLocation();

	// 公式：t = (d - O)・N / (D・N)
	// t = dot(PlaneNormal, PlanePoint - RayOrigin) / dot(PlaneNormal, RayDir)
	float Denom = FVector::DotProduct(WidgetForward, RayDir);

	// 平面が射線と平行な場合、交点は存在しない
	if (FMath::Abs(Denom) < KINDA_SMALL_NUMBER)
	{
		CursorWidget->SetHiddenInGame(true);
		return;
	}

	float T = FVector::DotProduct(WidgetForward, WidgetCenter - RayOrigin) / Denom;

	// Tがマイナスの場合、交点はプレイヤーの後ろにあるため、表示しない
	if (T < 0.0f)
	{
		CursorWidget->SetHiddenInGame(true);
		return;
	}

	FVector HitPoint = RayOrigin + RayDir * T;

	// オフセット方向
	FVector ToPlayer = (RayOrigin - HitPoint).GetSafeNormal();
	float Dot = FVector::DotProduct(WidgetForward, ToPlayer);
	FVector OffsetDirection = (Dot >= 0.0f) ? WidgetForward : -WidgetForward;

	FVector CursorPosition = HitPoint + OffsetDirection * CursorOffsetFromUI;
	FRotator CursorRotation = OffsetDirection.Rotation();

	CursorWidget->SetWorldLocation(CursorPosition);
	CursorWidget->SetWorldRotation(CursorRotation);
	CursorWidget->SetHiddenInGame(false);
}
