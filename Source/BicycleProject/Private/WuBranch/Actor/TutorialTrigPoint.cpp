// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/TutorialTrigPoint.h"
#include "Components/BoxComponent.h"
#include "WuBranch/Bike/BikeCharacter.h"
#include "WuBranch/System/Tutorialsystem.h"
//#include "WuBranch/MyGameInstance.h"

// Sets default values
ATutorialTrigPoint::ATutorialTrigPoint()
	: Type(ETutorialType::Controller)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATutorialTrigPoint::BeginPlay()
{
	Super::BeginPlay();
	
	if (FindTrigArea())
	{
		TrigArea->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBeginTriggerArea);
	}
}

// Called every frame
//void ATutorialTrigPoint::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

bool ATutorialTrigPoint::FindTrigArea()
{
	TrigArea = GetComponentByClass<UBoxComponent>();

	if (TrigArea)
		return true;

	return false;
}

void ATutorialTrigPoint::OnOverlapBeginTriggerArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABikeCharacter* Bike = Cast<ABikeCharacter>(OtherActor))
	{
		UTutorialsystem* TutorialSystem = GetGameInstance()->GetSubsystem<UTutorialsystem>();
		if (TutorialSystem && TutorialSystem->IsNeedTutorial())
		{
			TutorialSystem->OpenTutorial(Type);
		}
	}
}