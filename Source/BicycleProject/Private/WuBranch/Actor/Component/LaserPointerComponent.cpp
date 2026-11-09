// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/LaserPointerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values for this component's properties
ULaserPointerComponent::ULaserPointerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	LaserComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("Laser"));
	LaserComp->SetupAttachment(this);
	LaserComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserComp->SetCastShadow(false);
	LaserComp->SetGenerateOverlapEvents(false);
	LaserComp->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));

	DotComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("Laser Point"));
	DotComp->SetupAttachment(this);
	DotComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DotComp->SetCastShadow(false);
	DotComp->SetGenerateOverlapEvents(false);
	DotComp->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
}

// Called when the game starts
void ULaserPointerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	Interaction = FindInteraction();
	if (!Interaction)
	{ 
		SetComponentTickEnabled(false);
		return;
	}

	SetLaserVisible(false);
}

// Called every frame
void ULaserPointerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!Interaction || !LaserComp) 
		return;

	const FHitResult& Hit = Interaction->GetLastHitResult();
	const bool bOverUI = Interaction->IsOverInteractableWidget()
		|| Interaction->IsOverHitTestVisibleWidget();

	if (Interaction->GetHoveredWidgetComponent())
	{
		const float MyLength = (Hit.ImpactPoint - Hit.TraceStart).Length();
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("Hovered Widget: %s, Length: %lf, My Length: %lf"), *Interaction->GetHoveredWidgetComponent()->GetName(), Hit.Distance, MyLength));
	}
		
	const float Length = bOverUI ? Hit.Distance : Interaction->InteractionDistance;
	const float T = Thickness / 100.f;
	//LaserComp->SetRelativeLocation(FVector(Length * 0.5f, 0.f, 0.f));
	LaserComp->SetRelativeScale3D(FVector(T, T, Length / 100.f));

	if (!DotComp)
		return;
	DotComp->SetVisibility(bOverUI);
	if (bOverUI)
	{
		FVector Normal = Hit.ImpactNormal;
		// 当たった面と混ざってしまうので少し浮かせる
		DotComp->SetWorldLocation(Hit.ImpactPoint + Normal * 0.3f);
		DotComp->SetWorldRotation(FRotationMatrix::MakeFromZ(Normal).Rotator());
		DotComp->SetWorldScale3D(FVector(DotSize / 100.f));
	}
}

void ULaserPointerComponent::SetLaserVisible(bool bNewVisible)
{
	if (LaserComp)
		LaserComp->SetVisibility(bNewVisible);
	if (DotComp)
		DotComp->SetVisibility(bNewVisible);
	SetComponentTickEnabled(bNewVisible);
}

UWidgetInteractionComponent* ULaserPointerComponent::FindInteraction()
{
	USceneComponent* Parent = GetAttachParent();
	if (!Parent)
		return nullptr;

	for (USceneComponent* Child : Parent->GetAttachChildren())
	{
		if (UWidgetInteractionComponent* Found = Cast<UWidgetInteractionComponent>(Child))
		{
			return Found;
		}
	}

	return nullptr;
}

