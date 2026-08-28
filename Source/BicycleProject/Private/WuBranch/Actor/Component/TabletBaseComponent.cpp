// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/TabletBaseComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "WuBranch/Actor/Tablet.h"
#include "Engine/EngineTypes.h"

// Sets default values for this component's properties
UTabletBaseComponent::UTabletBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTabletBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	RecycleRange = FindRange(RecycleRangeTag);
	BaseRange = FindRange(BaseRangeTag);
	FindIconWidget();

	if (RecycleRange)
	{
		RecycleRange->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnRecycleRangeOverlapBegin);
	}

	if (BaseRange)
	{
		BaseRange->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOutsideBaseRange);
	}

	SpawnTablet();
	TabletCurrentState = ETabletState::Stay;
	Close();
}


// Called every frame
void UTabletBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
		
	OnMoveTablet(DeltaTime);
}

void UTabletBaseComponent::Open()
{
	DisableBaseRange();
	EnableRecycleRange();
	ShowRecycleIcon();
}

void UTabletBaseComponent::Close()
{
	EnableBaseRange();
	DisableRecycleRange();
	HideRecycleIcon();
}

void UTabletBaseComponent::ForceRecycleTablet()
{
	if (!Tablet)
		return;

	Tablet->StopNotify();
	SetTabletLocation(ETabletState::Highest);
	TabletCurrentState = ETabletState::Down;
	Tablet->CloseScene();
	Tablet->DisableCollision();
	Close();
}

void UTabletBaseComponent::StartUseTablet()
{
	TabletCurrentState = ETabletState::Up;
	if (Tablet)
		Tablet->Show();
}

USphereComponent* UTabletBaseComponent::FindRange(const FName& TargetTag)
{
	TArray<USceneComponent*> ChildComponents;
	GetChildrenComponents(true, ChildComponents);
	for (auto Child : ChildComponents)
	{
		if (USphereComponent* Range = Cast<USphereComponent>(Child))
		{
			if (Range->ComponentHasTag(TargetTag))
			{
				return Range;
			}
		}
	}
	
	UE_LOG(LogTemp, Error, TEXT("%s didnot attach"), *TargetTag.ToString());
	return nullptr;
}

void UTabletBaseComponent::FindIconWidget()
{
	TArray<USceneComponent*> ChildComponents;
	GetChildrenComponents(true, ChildComponents);
	for (auto Child : ChildComponents)
	{
		if (UWidgetComponent* Widget = Cast<UWidgetComponent>(Child))
		{
			if (Widget->ComponentHasTag(RecycleIconTag))
				RecycleIcon = Widget;
		}
	}
	if (!RecycleIcon)
		UE_LOG(LogTemp, Error, TEXT("%s didnot attach"), *RecycleIconTag.ToString());
}

void UTabletBaseComponent::OnRecycleRangeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ATablet* OverlappedTablet = Cast<ATablet>(OtherActor))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("Tablet in Recycle"));
		// タブレットが回収される
		// タブレットの位置を先に台座に固定してから下に移動させる
		SetTabletLocation(ETabletState::Highest);
		TabletCurrentState = ETabletState::Down;
		OverlappedTablet->CloseScene();
		OverlappedTablet->DisableCollision();

		Close();
	}
}

void UTabletBaseComponent::OnOutsideBaseRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ATablet* OverlappedTablet = Cast<ATablet>(OtherActor))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("OnOutsideBaseRange"));
		// タブレットが取られた
		Open();
		OverlappedTablet->OpenScene();
	}
}

void UTabletBaseComponent::SpawnTablet()
{
	FVector SpawnLocation = this->GetComponentLocation();
	FRotator SpawnRotation = FRotator(0.f, -90.f, -90.f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this->GetOwner();

	Tablet = GetWorld()->SpawnActor<ATablet>(TabletSample, SpawnLocation, SpawnRotation, SpawnParams);
	if (Tablet)
	{
		// タブレット台座の位置に置く
		SetTabletLocation(ETabletState::Lowest);
		Tablet->SetHideHeight(SpawnLocation.Z);
	}
}

void UTabletBaseComponent::SetTabletLocation(ETabletState State)
{
	if (!Tablet)
		return;

	FAttachmentTransformRules Rule(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);
	Tablet->AttachToActor(this->GetOwner(), Rule);
	FVector TabletBaseLocation = GetComponentLocation();
	if (State == ETabletState::Lowest)
		Tablet->SetActorLocation(TabletBaseLocation + TabletLowestLocation);
	else if (State == ETabletState::Highest)
		Tablet->SetActorLocation(TabletBaseLocation + TabletHighestLocation);

	Tablet->SetActorRelativeRotation(FRotator(0.f, -90.f, -90.f));
}

void UTabletBaseComponent::EnableRecycleRange()
{
	if (!RecycleRange)
		return;

	RecycleRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UTabletBaseComponent::DisableRecycleRange()
{
	if (!RecycleRange)
		return;

	RecycleRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UTabletBaseComponent::EnableBaseRange()
{
	if (!BaseRange)
		return;

	BaseRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UTabletBaseComponent::DisableBaseRange()
{
	if (!BaseRange)
		return;

	BaseRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UTabletBaseComponent::ShowRecycleIcon()
{
	if (!RecycleIcon)
		return;

	RecycleIcon->SetVisibility(true);
}

void UTabletBaseComponent::HideRecycleIcon()
{
	if (!RecycleIcon)
		return;

	RecycleIcon->SetVisibility(false);
}

void UTabletBaseComponent::OnMoveTablet(float DeltaTime)
{
	if (!Tablet)
	{
		UE_LOG(LogTemp, Error, TEXT("OnMoveTablet didnot have tablet"));
		return;
	}

	FVector Current = Tablet->GetActorLocation();

	// 移動の最終位置を決める。
	// このコンポーネントの位置から移動
	FVector Target = GetComponentLocation();
	if (TabletCurrentState == ETabletState::Up)
		Target += TabletHighestLocation;
	else if (TabletCurrentState == ETabletState::Down)
		Target += TabletLowestLocation;
	else
		return;

	// 移動
	FVector DeltaLocation = FMath::VInterpTo(Current, Target, DeltaTime, TabletMoveVelocity);
	Tablet->SetActorLocation(DeltaLocation);

	// 到着
	if (FVector::Dist(DeltaLocation, Target) < TabletMovementAccuracy)
	{
		// 再配置
		Tablet->SetActorLocation(Target);
		// 
		if (TabletCurrentState == ETabletState::Up)
		{
			TabletCurrentState = ETabletState::Highest;
			// フラッシュの通知エフェクト
			Tablet->FlashForNotify();

			Tablet->EnableCollision();
		}
		else if (TabletCurrentState == ETabletState::Down)
		{
			TabletCurrentState = ETabletState::Lowest;
			Tablet->Hide();
		}
	}
}
