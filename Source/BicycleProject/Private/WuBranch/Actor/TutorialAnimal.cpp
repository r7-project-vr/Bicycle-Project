// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/TutorialAnimal.h"
#include "Components/SkeletalMeshComponent.h"

ATutorialAnimal::ATutorialAnimal()
	: AWildAnimal()
	, bIsFadingInOut(false)
	, DissolveValue(MIN_DISSOLVE_VALUE)
	, DissolveSpeed(0.f)
	, TargetDissolveValue(MIN_DISSOLVE_VALUE)
	, DynMaterialInst(nullptr)
{
}

void ATutorialAnimal::BeginPlay()
{
	Super::BeginPlay();

}

void ATutorialAnimal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OnFadeOutComplete.IsBound())
		OnFadeOutComplete.Clear();

	Super::EndPlay(EndPlayReason);
}

void ATutorialAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsFadingInOut)
		Dissolve(DeltaTime);
}

void ATutorialAnimal::EnableHighlight_Implementation()
{
	if (!bIsFadingInOut)
		Super::EnableHighlight_Implementation();
}

void ATutorialAnimal::DisableHighlight_Implementation()
{
	if (!bIsFadingInOut)
		Super::DisableHighlight_Implementation();
}

void ATutorialAnimal::FadeIn()
{
	bIsFadingInOut = true;
	TargetDissolveValue = MIN_DISSOLVE_VALUE;
}

void ATutorialAnimal::FadeOut()
{
	bIsFadingInOut = true;
	TargetDissolveValue = MAX_DISSOLVE_VALUE;
}

void ATutorialAnimal::GetSelfMaterial()
{
	UMaterialInterface* Material = GetMesh()->GetMaterial(0);
	if (Material)
	{
		DynMaterialInst = GetMesh()->CreateDynamicMaterialInstance(0, Material);
		bool bSucc = Material->GetScalarParameterValue(FName("Dissolve"), DissolveValue);
		if (!bSucc)
			UE_LOG(LogTemp, Error, TEXT("Didnot find Dissolve Parameter in Current Material at Tutorial Animal."));
	}
}

void ATutorialAnimal::Dissolve(float DeltaTime)
{
	DissolveValue = FMath::FInterpTo(DissolveValue, TargetDissolveValue, DeltaTime, DissolveSpeed);
	ChangeDissolve(DissolveValue);

	// 完了した
	if (FMath::Abs(DissolveValue - TargetDissolveValue) < 0.1f)
	{
		DissolveValue = TargetDissolveValue;
		bIsFadingInOut = false;
		if (TargetDissolveValue == MAX_DISSOLVE_VALUE)
		{
			NotifyFadeOutComplete();
		}
	}
}

void ATutorialAnimal::ChangeDissolve(float Value)
{
	if (DynMaterialInst)
	{
		DynMaterialInst->SetScalarParameterValue(FName("Dissolve"), Value);
		GetMesh()->SetMaterial(0, DynMaterialInst);
	}
}

void ATutorialAnimal::NotifyFadeOutComplete()
{
	if (OnFadeOutComplete.IsBound())
	{
		OnFadeOutComplete.Broadcast();
	}
}
