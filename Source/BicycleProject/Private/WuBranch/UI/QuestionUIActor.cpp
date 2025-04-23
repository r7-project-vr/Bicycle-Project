// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/QuestionUIActor.h"
#include "Components/BoxComponent.h"
#include <WuBranch/Bike/BikeComponent.h>
#include <WuBranch/MyGameInstance.h>
#include "WuBranch/Device/DeviceManager.h"
#include "WuBranch/Device/Device.h"
#include <Kismet/KismetSystemLibrary.h>
#include <WuBranch/BikePlayerController.h>
#include <Kismet/GameplayStatics.h>

AQuestionUIActor::AQuestionUIActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_temporaryParkingArea = CreateDefaultSubobject<UBoxComponent>(FName("parkingArea"));
	_temporaryParkingArea->SetupAttachment(RootComponent);
	_temporaryParkingArea->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	_temporaryParkingArea->InitBoxExtent(FVector(100.0f, 32.0f, 5.0f));
	_temporaryParkingArea->OnComponentBeginOverlap.AddDynamic(this, &AQuestionUIActor::OnOverlapBeginParkingArea);
	AddInstanceComponent(_temporaryParkingArea);
}

void AQuestionUIActor::HandlePlayerEnterArea(UBikeComponent* bike)
{
	ABikePlayerController* pc = Cast<ABikePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (pc)
	{
		//pc->SetPlayerEnabledState(false);
	}

	// 自転車のスピードを強制的に0まで下げる
	bike->ReduceVelocityTo0();

	// プレイヤーに答えを選べるようにする
	UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	gameInstance->GetDeviceManager()->EnableSelectAnswerActions();
	gameInstance->GetDeviceManager()->BindSelectLeftEvent(bike, "OnSelectLeftAnswer");
	gameInstance->GetDeviceManager()->BindSelectRightEvent(bike, "OnSelectRightAnswer");
}

void AQuestionUIActor::OnOverlapBeginParkingArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		UKismetSystemLibrary::PrintString(this, "Start enter parking area", true, false, FColor::Red, 10.f);
		UBikeComponent* bike = OtherActor->GetComponentByClass<UBikeComponent>();
		HandlePlayerEnterArea(bike);
		// エリアのコリジョンを機能させない
		_temporaryParkingArea->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}
