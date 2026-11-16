// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/HologramComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "WuBranch/UI/TutorialMenuWidget.h"
#include "WuBranch/UI/TutorialIconWidget.h"
#include "NiagaraComponent.h"
#include "WuBranch/Bike/BikeCharacter.h"
#include "WuBranch/System/TutorialSystem.h"

// Sets default values for this component's properties
UHologramComponent::UHologramComponent()
	: HologramLine(nullptr)
	, TutorialWidget(nullptr)
	, TutorialScreen(nullptr)
	, TutorialIconWidget(nullptr)
	, TutorialIcon(nullptr)
	, CurrentProcess(HologramProcess::Stop)
	, ElapsedTime(0.f)
	, HologramLineLength(0.f)
	, CurrentLength(0.f)
	, ExtendTime(1.f)
	, HologramExpandAngle(0.f)
	, CurrentAngle(0.f)
	, ExpandTime(1.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

}


// Called when the game starts
void UHologramComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	TArray<USceneComponent*> ChildComponents;
	GetChildrenComponents(true, ChildComponents);
	TutorialScreen = FindScreen(ChildComponents);
	TutorialIcon = FindIcon(ChildComponents);
	HologramLine = FindHologramLine(ChildComponents);

	if (TutorialScreen)
	{
		TutorialScreen->TutorialChooseCompletedDelegate.AddDynamic(this, &UHologramComponent::OnScreenClose);
		InitializeTutorialScreen();
	}
}

// Called every frame
void UHologramComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	switch (CurrentProcess)
	{
	case HologramProcess::Extending:
		ExtendHologramLine(DeltaTime);
		break;
	case HologramProcess::Retracting:
		RetractHologramLine(DeltaTime);
		break;
	case HologramProcess::Expanding:
		ExpandHologramLine(DeltaTime);
		break;
	case HologramProcess::Collapsing:
		CollapseHologramLine(DeltaTime);
		break;
	}
}

void UHologramComponent::ActivateHologram()
{
	if (!HologramLine)
		return;
	
	if (HologramLineLength == 0.f)
		DecideTutorialScreenHeight();
	if (HologramExpandAngle == 0.f)
		CaculateExpandAngle();

	HologramLine->SetActive(true);
	CurrentLength = 0.f;
	CurrentAngle = 0.f;
	ChangeHologramAngle();
	CurrentProcess = HologramProcess::Extending;
	if (TutorialIcon)
		TutorialIcon->HideTutorialIcon();
	if (TutorialIconWidget)
		TutorialIconWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UHologramComponent::DeactivateHologram()
{
	if(CurrentLength != 0.f)
		OnScreenClose();
}

UTutorialMenuWidget* UHologramComponent::FindScreen(const TArray<USceneComponent*>& Childs)
{
	for (auto Child : Childs)
	{
		if (Child->ComponentHasTag(TutorialTag))
		{
			TutorialWidget = Cast<UWidgetComponent>(Child);
			if (TutorialWidget)
			{
				// この時、TutorialWidgetはまだ初期化していないため、GetUserWidgetObjectを使ってもnullptrがかえっただけ
				TutorialWidget->InitWidget();
				if (UTutorialMenuWidget* Screen = Cast<UTutorialMenuWidget>(TutorialWidget->GetUserWidgetObject()))
				{
					return Screen;
				}
			}
		}
	}
	return nullptr;
}

UTutorialIconWidget* UHologramComponent::FindIcon(const TArray<USceneComponent*>& Childs)
{
	for (auto Child : Childs)
	{
		if (Child->ComponentHasTag(TutorialIconTag))
		{
			TutorialIconWidget = Cast<UWidgetComponent>(Child);
			if (TutorialIconWidget)
			{
				TutorialIconWidget->InitWidget();
				if (UTutorialIconWidget* Icon = Cast<UTutorialIconWidget>(TutorialIconWidget->GetUserWidgetObject()))
				{
					return Icon;
				}
			}
		}
	}
	return nullptr;
}

UNiagaraComponent* UHologramComponent::FindHologramLine(const TArray<USceneComponent*>& Childs)
{
	for (auto Child : Childs)
	{
		if (UNiagaraComponent* Niagara = Cast<UNiagaraComponent>(Child))
		{
			return Niagara;
		}
	}
	return nullptr;
}

void UHologramComponent::InitializeTutorialScreen()
{
	if (UTutorialsystem* TutorialSystem = GetWorld()->GetGameInstance()->GetSubsystem<UTutorialsystem>())
	{
		if (TutorialScreen)
		{
			TArray<FTutorialData> Datas = TutorialSystem->GetTutorialDatas();
			TutorialScreen->SetMenu(Datas);
		}
	}
}

void UHologramComponent::DecideTutorialScreenHeight()
{
	if (TutorialWidget)
	{
		if (ABikeCharacter* Owner = Cast<ABikeCharacter>(GetOwner()))
		{
			float HeadHeight = Owner->GetHeadHeight();
			// チュートリアルスクリーンをプレイヤーの目と同じ高さにする
			FVector WidgetLocation = TutorialWidget->GetComponentLocation();
			WidgetLocation.Z = HeadHeight;
			TutorialWidget->SetWorldLocation(WidgetLocation);
			// 線の長さを設定
			// 目の高さ - 伸縮後のウィジェットの長さの半分 - コンポーネントの高さ
			float WidgetHeight = TutorialWidget->GetDrawSize().Y;
			float LocalScale = TutorialWidget->GetComponentScale().Z;
			HologramLineLength = HeadHeight - WidgetHeight * LocalScale / 2 - GetComponentLocation().Z;
		}
	}
}

void UHologramComponent::CaculateExpandAngle()
{
	if (TutorialWidget)
	{
		if (ABikeCharacter* Owner = Cast<ABikeCharacter>(GetOwner()))
		{
			float HeadHeight = Owner->GetHeadHeight();
			FVector2D WidgetDrawSize = TutorialWidget->GetDrawSize();
			FVector Scale = TutorialWidget->GetComponentScale();
			FVector Start = GetComponentLocation();
			FVector End = FVector(Start.X, Start.Y + WidgetDrawSize.X * Scale.Y / 2, HeadHeight - WidgetDrawSize.Y * Scale.Z / 2);
			FVector Dir = End - Start;
			HologramExpandAngle = FMath::RadiansToDegrees(FMath::Atan2(Dir.Y, Dir.Z));
		}
	}
}

void UHologramComponent::ExtendHologramLine(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(ElapsedTime / ExtendTime, 0.f, 1.0f);
	CurrentLength = FMath::InterpEaseInOut(0.f, HologramLineLength, Alpha, 1.0f);
	//Alpha = ExtendCurve->GetFloatValue(Alpha);
	//CurrentLength = FMath::Lerp(0.f, HologramLineLength, Alpha);
	ChangeHologramAngle();

	if (ElapsedTime >= ExtendTime)
	{
		ElapsedTime = 0.f;
		CurrentProcess = HologramProcess::Expanding;
	}
}

void UHologramComponent::RetractHologramLine(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(ElapsedTime / ExtendTime, 0.f, 1.0f);
	CurrentLength = FMath::InterpEaseInOut(HologramLineLength, 0.f, Alpha, 1.0f);
	//Alpha = ExtendCurve->GetFloatValue(Alpha);
	//CurrentLength = FMath::Lerp(HologramLineLength, 0.f, Alpha);
	ChangeHologramAngle();

	if (ElapsedTime >= ExtendTime)
	{
		ElapsedTime = 0.f;
		CurrentProcess = HologramProcess::Stop;
		if(HologramLine)
			HologramLine->SetActive(false);
		if (TutorialIcon)
			TutorialIcon->ShowTutorialIcon();
		if (TutorialIconWidget)
			TutorialIconWidget->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void UHologramComponent::ExpandHologramLine(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(ElapsedTime / ExpandTime, 0.f, 1.0f);
	CurrentAngle = FMath::InterpEaseInOut(0.f, HologramExpandAngle, Alpha, 1.0f);
	ChangeHologramAngle();

	// 完全展開した
	if (ElapsedTime >= ExpandTime)
	{
		ElapsedTime = 0.f;
		CurrentProcess = HologramProcess::Stop;
		ShowTutorialScreen();
	}
}

void UHologramComponent::CollapseHologramLine(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(ElapsedTime / ExpandTime, 0.f, 1.0f);
	CurrentAngle = FMath::InterpEaseInOut(HologramExpandAngle, 0.f, Alpha, 1.0f);
	ChangeHologramAngle();

	// 完全に閉じた
	if (ElapsedTime >= ExpandTime)
	{
		ElapsedTime = 0.f;
		CurrentProcess = HologramProcess::Retracting;
	}
}

void UHologramComponent::ChangeHologramAngle()
{
	if (!HologramLine)
		return;

	FVector EndLocation;
	// 左側
	float RAngle = FMath::DegreesToRadians(CurrentAngle + BASE_ANGLE);
	float EndY = FMath::Cos(RAngle) * CurrentLength;
	float EndZ = FMath::Sin(RAngle) * CurrentLength;
	HologramLine->SetVariableVec3(FName("User.EndLocationLeft"), FVector(0.f, EndY, EndZ));
	// 右側
	RAngle = FMath::DegreesToRadians(-CurrentAngle + BASE_ANGLE);
	EndY = FMath::Cos(RAngle) * CurrentLength;
	EndZ = FMath::Sin(RAngle) * CurrentLength;
	HologramLine->SetVariableVec3(FName("User.EndLocationRight"), FVector(0.f, EndY, EndZ));
}

void UHologramComponent::ShowTutorialScreen()
{
	if (!TutorialWidget || !TutorialScreen)
		return;

	TutorialWidget->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TutorialScreen->ShowScene();
}

void UHologramComponent::OnScreenClose()
{
	if (!TutorialWidget || !TutorialScreen)
		return;

	TutorialWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TutorialScreen->CloseScene();
	CurrentProcess = HologramProcess::Collapsing;
}

