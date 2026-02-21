// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/QuestionUIActor.h"
#include "Components/BoxComponent.h"
#include <WuBranch/Bike/BikeComponent.h>
#include <WuBranch/Bike/BikeCharacter.h>
#include <WuBranch/Bike/BikeMovementComponent.h>
#include <WuBranch/MyGameInstance.h>
#include "WuBranch/Device/DeviceManager.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Components/SplineComponent.h>
#include <WuBranch/QuestionGameMode.h>
#include <Components/WidgetComponent.h>
#include <ShiiBranch/OptionUIWidget.h>
#include "UntakuBranch/Question.h"
#include "Kismet/GameplayStatics.h" 
#include "narisawaBranch/ProceduralRoadGenerator.h" 


AQuestionUIActor::AQuestionUIActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SnapPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SnapPoint"));
	//SnapPoint->SetupAttachment(RootComponent);
	RootComponent = SnapPoint;

	_temporaryParkingArea = CreateDefaultSubobject<UBoxComponent>(FName("parkingArea"));
	_temporaryParkingArea->SetupAttachment(RootComponent);
	_temporaryParkingArea->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	_temporaryParkingArea->InitBoxExtent(FVector(100.0f, 64.0f, 5.0f));

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

	MessageBoard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Message board"));
	MessageBoard->SetupAttachment(RootComponent);

	Widget->SetupAttachment(RootComponent);
}

FVector AQuestionUIActor::GetSnapLocation() const
{
	return SnapPoint->GetComponentLocation();
}

FRotator AQuestionUIActor::GetSnapRotation() const
{
	return SnapPoint->GetComponentRotation();
}

void AQuestionUIActor::BeginPlay()
{
	Super::BeginPlay();

	SetTarget(nullptr);
	_exitTarget = nullptr;
	MovedDistance = 0.0f;
	bIsAnswered = false;
	if (_temporaryParkingArea)
	{
		_temporaryParkingArea->OnComponentBeginOverlap.AddDynamic(this, &AQuestionUIActor::OnOverlapBeginParkingArea);
		_temporaryParkingArea->OnComponentEndOverlap.AddDynamic(this, &AQuestionUIActor::OnOverlapEndParkingArea);
	}

	NotDisplayUI();
}

void AQuestionUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LeadToExit(DeltaTime);
}

void AQuestionUIActor::UseLeftExit()
{
	bIsAnswered = true;
	_exitTarget = _exitLeft;

	// レベル内に配置された AProceduralRoadGenerator を探す
	AProceduralRoadGenerator* Generator = Cast<AProceduralRoadGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AProceduralRoadGenerator::StaticClass()));
	if (Generator)
	{
		// 左の出口スプラインを渡して、建物生成を依頼する
		Generator->GenerateBuildingsAlongPath(_exitLeft);
	}
}

void AQuestionUIActor::UseRightExit()
{
	bIsAnswered = true;
	_exitTarget = _exitRight;

	// レベル内に配置された AProceduralRoadGenerator を探す
	AProceduralRoadGenerator* Generator = Cast<AProceduralRoadGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AProceduralRoadGenerator::StaticClass()));
	if (Generator)
	{
		// 右の出口スプラインを渡して、建物生成を依頼する
		Generator->GenerateBuildingsAlongPath(_exitRight);
	}
}

bool AQuestionUIActor::GetAnsweredStatus() const
{
	return bIsAnswered;
}

void AQuestionUIActor::DisableFeature()
{
	// UI
	NotDisplayUI();
	// コリジョン
	DisableCollision();
}

bool AQuestionUIActor::GetExitLocationAndForward(FVector& oLocation, FVector& oForward)
{
	//出口がある
	if (!_exitTarget)
		return false;
	int ExitNum = _exitTarget->GetNumberOfSplinePoints();
	if (ExitNum > 1)
	{
		oLocation = _exitTarget->GetLocationAtSplinePoint(ExitNum - 1, ESplineCoordinateSpace::World);
		oForward = _exitTarget->GetDirectionAtSplinePoint(ExitNum - 1, ESplineCoordinateSpace::World);
		return true;
	}
	return false;
}

void AQuestionUIActor::GetQuestionFromManger()
{
	if (AQuestionGameMode* GameMode = Cast<AQuestionGameMode>(GetWorld()->GetAuthGameMode()))
		Question = GameMode->GetQuestion();
}

FQuestion* AQuestionUIActor::GetNowQuestion()
{
	return Question;
}

void AQuestionUIActor::HandlePlayerEnterArea(ABikeCharacter* Bike)
{
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	// デフォルトアクションを機能させない
	GameInstance->GetDeviceManager()->DisableDefaultActions();
	// プレイヤーに答えを選べるようにする
	GameInstance->GetDeviceManager()->EnableSelectAnswerActions();

	// 自転車のスピードを強制的に0まで下げる
	Bike->StopMove();

	// オートプレイのスタート地点へ誘導
	Bike->EnableAutoPlay(this);
	if (UBikeMovementComponent* Movement = Bike->GetComponentByClass<UBikeMovementComponent>())
	{
		Movement->OnArrivedLocationEvent.AddDynamic(this, &AQuestionUIActor::OnArrivedEnterLocation);
		FVector Pos = _autoPlayStart->GetWorldLocationAtSplinePoint(0);
		Movement->SetSynchPos(Pos);
	}
	/*Bike->OnArrivedLocationEvent.AddDynamic(this, &AQuestionUIActor::OnArrivedEnterLocation);
	FVector Pos = _autoPlayStart->GetWorldLocationAtSplinePoint(0);
	Bike->SetSynchPos(Pos);*/
}

void AQuestionUIActor::OnOverlapBeginParkingArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsAnswered && OtherActor->ActorHasTag("Player"))
	{
		//UKismetSystemLibrary::PrintString(this, "Start enter parking area", true, false, FColor::Green, 10.f);
		
		SetQuiz();

		// オートプレイ対象の設置
		// 自転車の移動機能をBikeMovementComponentに移動した
		if (ABikeCharacter* Bike = Cast<ABikeCharacter>(OtherActor))
		{
			SetTarget(Bike);
			HandlePlayerEnterArea(Bike);
		}
		/*UBikeComponent* Bike = OtherActor->GetComponentByClass<UBikeComponent>();
		SetTarget(Bike);
		HandlePlayerEnterArea(Bike);*/
	}
}

void AQuestionUIActor::OnOverlapEndParkingArea(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bIsAnswered && OtherActor->ActorHasTag("Player"))
	{
		DisableFeature();
	}
}

void AQuestionUIActor::SetQuiz()
{
	if (UOptionUIWidget* UI = Cast<UOptionUIWidget>(Widget->GetWidget()))
	{
		UI->SetQuestionAndAnswer(*Question);
	}
}

void AQuestionUIActor::OnArrivedEnterLocation()
{
	DisplayUI();
	if (UBikeMovementComponent* Movement = AutoPlayTarget->GetComponentByClass<UBikeMovementComponent>())
		Movement->OnArrivedLocationEvent.RemoveDynamic(this, &AQuestionUIActor::OnArrivedEnterLocation);
	//Bike->OnArrivedLocationEvent.RemoveDynamic(this, &AQuestionUIActor::OnArrivedEnterLocation);
}

void AQuestionUIActor::SetTarget(ABikeCharacter* Target)
{
	AutoPlayTarget = Target;
}

void AQuestionUIActor::LeadToExit(float DeltaTime)
{
	if (_exitTarget != nullptr)
	{
		// spline pointが一つ以下の時無視、出口設置していないので
		if (_exitTarget->GetNumberOfSplinePoints() <= 1)
			return;

		float DeltaDis = DeltaTime * _autoPlayMoveSpeed;
		MovedDistance += DeltaDis;
		//FTransform pos = _exitTarget->GetTransformAtDistanceAlongSpline(_movedDistance, ESplineCoordinateSpace::Type::World);
		FVector Pos = _exitTarget->GetLocationAtDistanceAlongSpline(MovedDistance, ESplineCoordinateSpace::Type::World);
		if (AutoPlayTarget)
		{
			if(UBikeMovementComponent* Movement = AutoPlayTarget->GetComponentByClass<UBikeMovementComponent>())
				Movement->SetSynchPos(Pos);
		}
		// 出口に着いた
		if (MovedDistance > _exitTarget->GetSplineLength())
		{
			_exitTarget = nullptr;
			// オートプレイ解除
			if (AutoPlayTarget)
				AutoPlayTarget->DisableAutoPlay();
		}
	}
}

void AQuestionUIActor::DisableCollision()
{
	// エリアのコリジョン
	_temporaryParkingArea->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

FVector AQuestionUIActor::GetSnapLocation_Implementation() const
{
	// 右出口スプラインがあり、ポイントが1つ以上存在するなら
	if (_exitRight && _exitRight->GetNumberOfSplinePoints() > 0)
	{
		// スプラインの終点のワールド位置を返す
		const int32 LastPointIndex = _exitRight->GetNumberOfSplinePoints() - 1;
		return _exitRight->GetLocationAtSplinePoint(LastPointIndex, ESplineCoordinateSpace::World);
	}

	// もしスプラインがなければ、アクター自身の位置を返す
	return GetActorLocation();
}

FRotator AQuestionUIActor::GetSnapRotation_Implementation() const
{
	if (_exitRight && _exitRight->GetNumberOfSplinePoints() > 0)
	{
		// スプラインの終点のワールドでの向きを返す
		const int32 LastPointIndex = _exitRight->GetNumberOfSplinePoints() - 1;
		return _exitRight->GetRotationAtSplinePoint(LastPointIndex, ESplineCoordinateSpace::World);
	}

	// アクター自身の向きを返す
	return GetActorRotation();
}

void AQuestionUIActor::SetResult(int AnswerIndex, bool Result)
{
	if (UOptionUIWidget* UI = Cast<UOptionUIWidget>(Widget->GetWidget()))
	{
		UI->SetResult(AnswerIndex, Result);
	}
}
