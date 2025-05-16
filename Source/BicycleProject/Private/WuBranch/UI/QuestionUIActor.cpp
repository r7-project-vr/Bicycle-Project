// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/QuestionUIActor.h"
#include "Components/BoxComponent.h"
#include <WuBranch/Bike/BikeComponent.h>
#include <WuBranch/MyGameInstance.h>
#include "WuBranch/Device/DeviceManager.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Components/SplineComponent.h>


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

	_autoPlayStart = CreateDefaultSubobject<USplineComponent>(FName("AutoPlayStart"));
	_autoPlayStart->SetupAttachment(RootComponent);
	_autoPlayStart->SetRelativeLocation(FVector(700.0f, 0.0f, 90.0f));
	_autoPlayStart->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

	_exitLeft = CreateDefaultSubobject<USplineComponent>(FName("ExitLeft"));
	_exitLeft->SetupAttachment(RootComponent);
	_exitLeft->SetRelativeLocation(FVector(600.0f, 0.0f, 90.0f));
	_exitLeft->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

	_exitRight = CreateDefaultSubobject<USplineComponent>(FName("ExitRight"));
	_exitRight->SetupAttachment(RootComponent);
	_exitRight->SetRelativeLocation(FVector(600.0f, 0.0f, 90.0f));
	_exitRight->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	
	_autoPlayMoveSpeed = 10.0f;
}

void AQuestionUIActor::BeginPlay()
{
	Super::BeginPlay();

	SetTarget(nullptr);
	_exitTarget = nullptr;
	_movedDistance = 0.0f;
	_isAnswered = false;

	NotDisplayUI();
}

void AQuestionUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LeadToExit(DeltaTime);
}

void AQuestionUIActor::SetProblem()
{
}

void AQuestionUIActor::UseLeftExit()
{
	_isAnswered = true;
	_exitTarget = _exitLeft;
	UpdateStatus();
}

void AQuestionUIActor::UseRightExit()
{
	_isAnswered = true;
	_exitTarget = _exitRight;
	UpdateStatus();
}

bool AQuestionUIActor::GetAnsweredStatus() const
{
	return _isAnswered;
}

void AQuestionUIActor::DisableFeature()
{
	// UI
	NotDisplayUI();
	// コリジョン
	DisableCollision();
}

void AQuestionUIActor::HandlePlayerEnterArea(UBikeComponent* bike)
{
	UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	// デフォルトアクションを機能させない
	gameInstance->GetDeviceManager()->DisableDefaultActions();
	// プレイヤーに答えを選べるようにする
	gameInstance->GetDeviceManager()->EnableSelectAnswerActions();

	// 自転車のスピードを強制的に0まで下げる
	bike->ReduceVelocityTo0();

	// オートプレイのスタート地点へ誘導
	bike->EnableAutoPlay(this);
	FVector pos = _autoPlayStart->GetWorldLocationAtSplinePoint(0);
	bike->SetSynchPos(pos);
}

void AQuestionUIActor::OnOverlapBeginParkingArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		UKismetSystemLibrary::PrintString(this, "Start enter parking area", true, false, FColor::Green, 10.f);
		
		UBikeComponent* bike = OtherActor->GetComponentByClass<UBikeComponent>();
		SetTarget(bike);
		HandlePlayerEnterArea(bike);
		
		DisableCollision();
	}
}

void AQuestionUIActor::SetTarget(UBikeComponent* target)
{
	_autoPlayTarget = target;
}

void AQuestionUIActor::LeadToExit(float DeltaTime)
{
	if (_exitTarget != nullptr)
	{
		// spline pointが一つ以下の時無視、出口設置していないので
		if (_exitTarget->GetNumberOfSplinePoints() <= 1)
			return;
		
		float deltaDis = DeltaTime * _autoPlayMoveSpeed;
		_movedDistance += deltaDis;
		//FTransform pos = _exitTarget->GetTransformAtDistanceAlongSpline(_movedDistance, ESplineCoordinateSpace::Type::World);
		FVector pos = _exitTarget->GetLocationAtDistanceAlongSpline(_movedDistance, ESplineCoordinateSpace::Type::World);
		if (_autoPlayTarget)
		{
			_autoPlayTarget->SetSynchPos(pos);
		}
		// 出口に着いた
		if (_movedDistance > _exitTarget->GetSplineLength())
		{
			_exitTarget = nullptr;
			// オートプレイ解除
			_autoPlayTarget->DisableAutoPlay();
			// デフォルトアクションを機能させる
			UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
			gameInstance->GetDeviceManager()->EnableDefaultActions();
		}
	}
}

void AQuestionUIActor::UpdateStatus()
{
	//回答済み
	if (_isAnswered)
	{
		DisableFeature();
	}
}

void AQuestionUIActor::DisableCollision()
{
	// エリアのコリジョン
	_temporaryParkingArea->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}
