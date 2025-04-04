// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/BaseUIPawn.h"
#include <Components/WidgetComponent.h>
#include "Components/BoxComponent.h"
#include <WuBranch/Bike/BikeComponent.h>
#include <Kismet/KismetSystemLibrary.h>

// Sets default values
ABaseUIPawn::ABaseUIPawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	AddInstanceComponent(RootComponent);

	_widget = CreateDefaultSubobject<UWidgetComponent>(FName("widget"));
	_widget->SetupAttachment(RootComponent);
	AddInstanceComponent(_widget);

	_temporaryParkingArea = CreateDefaultSubobject<UBoxComponent>(FName("parkingArea"));
	_temporaryParkingArea->SetupAttachment(RootComponent);
	_temporaryParkingArea->SetRelativeLocation(FVector(32.0f, 0.0f, 0.0f));
	_temporaryParkingArea->InitBoxExtent(FVector(32.0f, 32.0f, 5.0f));
	_temporaryParkingArea->OnComponentBeginOverlap.AddDynamic(this, &ABaseUIPawn::OnOverlapBeginParkingArea);
	AddInstanceComponent(_temporaryParkingArea);

}

// Called when the game starts or when spawned
void ABaseUIPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseUIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseUIPawn::OnOverlapBeginParkingArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		UKismetSystemLibrary::PrintString(this, "Player enter area!", true, false, FColor::Orange, 100.f);
		UBikeComponent* bike = OtherActor->GetComponentByClass<UBikeComponent>();
		bike->OpenForcedControl();
		bike->ReduceVelocityTo0();
	}
}

