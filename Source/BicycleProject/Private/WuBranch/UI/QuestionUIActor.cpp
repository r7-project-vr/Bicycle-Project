// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/UI/QuestionUIActor.h"
#include "Components/BoxComponent.h"
#include <WuBranch/Bike/BikeComponent.h>
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
	SnapPoint->SetupAttachment(RootComponent);
	RootComponent = SnapPoint;

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
	_movedDistance = 0.0f;
	_isAnswered = false;
	_isGameFinished = false;

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

	// レベル内に配置された AProceduralRoadGenerator を探す
	AProceduralRoadGenerator* Generator = Cast<AProceduralRoadGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AProceduralRoadGenerator::StaticClass()));
	if (Generator)
	{
		// 左の出口スプラインを渡して、建物生成を依頼する
		Generator->GenerateBuildingsAlongPath(_exitLeft);
	}

	UpdateStatus();
}

void AQuestionUIActor::UseRightExit()
{
	_isAnswered = true;
	_exitTarget = _exitRight;

	// レベル内に配置された AProceduralRoadGenerator を探す
	AProceduralRoadGenerator* Generator = Cast<AProceduralRoadGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AProceduralRoadGenerator::StaticClass()));
	if (Generator)
	{
		// 右の出口スプラインを渡して、建物生成を依頼する
		Generator->GenerateBuildingsAlongPath(_exitRight);
	}

	UpdateStatus();
}

bool AQuestionUIActor::GetAnsweredStatus() const
{
	return _isAnswered;
}

void AQuestionUIActor::DisableFeature()
{
	_isGameFinished = true;
	// UI
	NotDisplayUI();
	// コリジョン
	DisableCollision();
}

bool AQuestionUIActor::GetExitLocationAndForward(FVector& oLocation, FVector& oForward)
{
	//出口がある
	int exitNum = _exitTarget->GetNumberOfSplinePoints();
	if (exitNum > 1)
	{
		oLocation = _exitTarget->GetLocationAtSplinePoint(exitNum - 1, ESplineCoordinateSpace::World);
		oForward = _exitTarget->GetDirectionAtSplinePoint(exitNum - 1, ESplineCoordinateSpace::World);
		return true;
	}
	return false;
}

FQuestion* AQuestionUIActor::GetNowQuestion()
{
	return _questionTmp;
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
	if (!_isGameFinished && OtherActor->ActorHasTag("Player"))
	{
		UKismetSystemLibrary::PrintString(this, "Start enter parking area", true, false, FColor::Green, 10.f);
		
		// 問題内容をゲット
		AQuestionGameMode* gameMode = Cast<AQuestionGameMode>(GetWorld()->GetAuthGameMode());
		FQuestion* question = gameMode->GetQuestion();
		_questionTmp = question;
		// 問題UIにデータを渡す
		if (UOptionUIWidget* UI = Cast<UOptionUIWidget>(_widget->GetWidget()))
		{
			UI->SetQuestionAndAnswer(*question);
		}

		// オートプレイ対象の設置
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
