// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/QuestionUIActor.h"
#include "Components/BoxComponent.h"
#include <WuBranch/Bike/BikeComponent.h>
#include <Kismet/KismetSystemLibrary.h>

AQuestionUIActor::AQuestionUIActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_temporaryParkingArea = CreateDefaultSubobject<UBoxComponent>(FName("parkingArea"));
	_temporaryParkingArea->SetupAttachment(RootComponent);
	_temporaryParkingArea->SetRelativeLocation(FVector(32.0f, 0.0f, 0.0f));
	_temporaryParkingArea->InitBoxExtent(FVector(32.0f, 32.0f, 5.0f));
	_temporaryParkingArea->OnComponentBeginOverlap.AddDynamic(this, &AQuestionUIActor::OnOverlapBeginParkingArea);
	AddInstanceComponent(_temporaryParkingArea);
}

void AQuestionUIActor::OnOverlapBeginParkingArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		UKismetSystemLibrary::PrintString(this, "Player enter area!", true, false, FColor::Orange, 100.f);
		UBikeComponent* bike = OtherActor->GetComponentByClass<UBikeComponent>();
		bike->OpenForcedControl();
		bike->ReduceVelocityTo0();
	}
}
