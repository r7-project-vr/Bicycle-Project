// Fill out your copyright notice in the Description page of Project Settings.

#include "WuBranch/Bike/BikeCharacter.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Device/DeviceManager.h"
#include "WuBranch/Bike/BikeComponent.h"
#include "WuBranch/Bike/WidgetInteractionHeadComponent.h"
#include "WuBranch/Actor/Component/AnimalManagerComponent.h"
#include "WuBranch/Bike/BikeMovementComponent.h"
#include "WuBranch/Bike/ResponderComponent.h"
#include "WuBranch/Actor/Component/PhotoCaptureComponent.h"
#include "Kismet/GameplayStatics.h"
#include <WuBranch/UI/QuestionUIActor.h>
#include <WuBranch/Actor/Component/PositionResetComponent.h>
#include <WuBranch/Bike/MyHandRootComponent.h>
#include "WuBranch/Actor/Tablet.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Components/WidgetComponent.h"

// Sets default values
ABikeCharacter::ABikeCharacter()
	: IsOverSpeed(false)
	, IsPause(false)
	, LeftHandRoot(nullptr)
	, RightHandRoot(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	Bike = CreateDefaultSubobject<UBikeComponent>(TEXT("Bike"));
	AnimalManager = CreateDefaultSubobject<UAnimalManagerComponent>(TEXT("Animal Manager"));
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	TabletBase = CreateDefaultSubobject<UBoxComponent>(TEXT("TabletBase"));
	TabletBase->SetupAttachment(GetMesh());
	TabletBase->SetCollisionResponseToChannels(ECR_Ignore);
	TabletBase->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	_handlebarCenteringSpeed = 1.0f;
	_isRotate = false;
	HandleBarsAngle = 0.0f;
}

// Called when the game starts or when spawned
void ABikeCharacter::BeginPlay()
{
	Super::BeginPlay();

	FindMover();
	FindResponder();
	FindPhotoCapture();
	FindHandStretcher();
	FindTabletBaseIcon();
	_widgetInteractionHeadComponent = GetComponentByClass<UWidgetInteractionHeadComponent>();
	LoadBikeMesh();
	_targetRotator = FRotator::ZeroRotator;
	IsOverSpeed = false;
	IsPause = false;
	TabletCurrentState = ETabletState::Stay;

	// PhotoCaptureBoxを初期化
	if (PhotoCapture)
	{
		PhotoCapture->InitializePhotoCaptureBox(RootComponent);
	}

	if (TabletBase)
	{
		TabletBase->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnTabletBaseOverlapBegin);
	}
	CloseTabletBase();

	// ゲーム開始時にスクリーンショットをリセット
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		GameInstance->ResetScreenshots();
	}

	/*FTimerHandle TutorialTimer;
	GetWorld()->GetTimerManager().SetTimer(TutorialTimer, [this]() {
		OpenTutorial(ETutorialType::Quiz);
		}, 5.0f, false);*/
}

// Called every frame
void ABikeCharacter::Tick(float DeltaTime)
{
	if (IsPause)
		return;

	Super::Tick(DeltaTime);
	
	RotateBike(DeltaTime);

	if (CheckOverSpeed())
	{
		IsOverSpeed = true;
	}

	MoveUpTablet(DeltaTime);
	MoveDownTablet(DeltaTime);
}

// Called to bind functionality to input
void ABikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Get Game Instance Error!"));
	}
	else
	{
		if (UDeviceManager* DeviceManager = GameInstance->GetDeviceManager())
		{
			//DeviceManager->CreateAllDevices();
			DeviceManager->EnableAllDevices();
			/*if (Bike)
			{
				DeviceManager->BindMoveEvent(Bike, "OnMove");
				DeviceManager->BindSelectLeftEvent(Bike, "OnSelectLeftAnswer");
				DeviceManager->BindSelectRightEvent(Bike, "OnSelectRightAnswer");
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Bind event")));
			}*/
			if (!BikeMovement)
				FindMover();
			if (BikeMovement)
			{
				DeviceManager->BindMoveEvent(BikeMovement, "OnMove");
			}
			if (!Responder)
				FindResponder();
			if (Responder)
			{
				DeviceManager->BindSelectionEvent(Responder, "OnSelectAnswer");
				DeviceManager->BindDecisionEvent(Responder, "OnDecision");
			}

			// BikeCharacterで撮影を行う
			if (!PhotoCapture)
				FindPhotoCapture();
            if (PhotoCapture)
		    {
			    DeviceManager->BindScreenshotEvent(PhotoCapture, FName("OnScreenshotTaken"));
				//DeviceManager->BindPhotoReadyEvent(PhotoCapture, "");
            }

			if (!LeftHandRoot || !RightHandRoot)
				FindHandStretcher();
			if (LeftHandRoot && RightHandRoot)
			{
				DeviceManager->BindLeftHandStretchEvent(LeftHandRoot, "OnStretchArm");
				DeviceManager->BindRightHandStretchEvent(RightHandRoot, "OnStretchArm");
			}

			DeviceManager->EnableDefaultActions();
			DeviceManager->EnableTakePhotoActions();
			DeviceManager->EnableStretchActions();
        }
	}
}

void ABikeCharacter::Pause_Implementation()
{
	IsPause = true;
	if (!BikeMovement->GetIsAutoPlay())
	{
		UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
		if (GameInstance)
		{
			if (UDeviceManager* DeviceManager = GameInstance->GetDeviceManager())
			{
				DeviceManager->DisableDefaultActions();
				DeviceManager->DisableTakePhotoActions();

				if (BikeMovement->GetIsAutoPlay())
					DeviceManager->DisableSelectAnswerActions();
			}
		}
		StopMove();
	}
}

void ABikeCharacter::ReStart_Implementation()
{
	IsPause = false;
	if (!BikeMovement->GetIsAutoPlay())
	{
		UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
		if (GameInstance)
		{
			if (UDeviceManager* DeviceManager = GameInstance->GetDeviceManager())
			{
				DeviceManager->EnableDefaultActions();
				DeviceManager->EnableTakePhotoActions();

				if (BikeMovement->GetIsAutoPlay())
					DeviceManager->DisableSelectAnswerActions();
			}
		}
	}
}

bool ABikeCharacter::IsPause_Implementation()
{
	return IsPause;
}

void ABikeCharacter::ChangeBikeMesh()
{

}

float ABikeCharacter::GetSpeed()
{
	return GetCharacterMovement()->Velocity.Length();
}

float ABikeCharacter::GetHandlerAngle() const
{
	return HandleBarsAngle;
}

void ABikeCharacter::SetTurningAngle(FRotator Angle)
{
	_targetRotator = GetActorRotation() + Angle;
	// 右折
	if (Angle.Yaw > 0)
		HandleBarsAngle = -30.0f;
	// 左折
	else if (Angle.Yaw < 0)
		HandleBarsAngle = 30.0f;
	_isRotate = true;
}

void ABikeCharacter::DisableHintLine()
{
	if (_widgetInteractionHeadComponent)
	{
		_widgetInteractionHeadComponent->DisableHintLine();
	}
}

void ABikeCharacter::StopMove()
{
	if (BikeMovement)
		BikeMovement->ReduceVelocityTo0();
}

bool ABikeCharacter::HasOverSpeed() const
{
	return IsOverSpeed;
}

void ABikeCharacter::ResetOverSpeed()
{
	IsOverSpeed = false;
}

void ABikeCharacter::LoadBikeMesh()
{	
	if (_bikeSkeletalNeedLoad)
	{
		const FSoftObjectPath& AssetRef = _bikeSkeletalNeedLoad.ToSoftObjectPath();
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(AssetRef, FStreamableDelegate::CreateUObject(this, &ABikeCharacter::LoadMeshComplete));
	}
}

void ABikeCharacter::LoadMeshComplete()
{
	USkeletalMesh* SkeletalMesh = _bikeSkeletalNeedLoad.Get();
	if (SkeletalMesh)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMesh);
		SpawnTablet();
	}
}

void ABikeCharacter::SpawnTablet()
{
	const USkeletalMeshSocket* TabletSocket = GetMesh()->GetSocketByName(FName("TabletSocket"));
	if (!TabletSocket || !TabletSample)
		return;

	FVector SpawnLocation = TabletSocket->GetSocketLocation(GetMesh());
	FRotator SpawnRotation = FRotator(180.f, 0.f, 30.f);

	FActorSpawnParameters  SpawnParams;
	SpawnParams.Owner = this;

	Tablet = GetWorld()->SpawnActor<ATablet>(TabletSample, SpawnLocation, SpawnRotation,SpawnParams);
	if (Tablet)
	{
		TabletSocket->AttachActor(Tablet, GetMesh());
		Tablet->SetActorRelativeLocation(TabletLowestLocation);
		Tablet->SetHideHeight(SpawnLocation.Z);
	}
}

void ABikeCharacter::RotateBike(float DeltaTime)
{
	if (!_isRotate)
		return;

	FRotator Current = GetActorRotation();
	// 曲がった
	if (Current.Equals(_targetRotator, 0.5f))
	{
		// 0.5度未満の時は曲がり終了と見なすため、強制的に角度を最終角度に設定
		SetActorRelativeRotation(_targetRotator);
		HandleBarsAngle = 0.0f;
		_isRotate = false;
		// 強制コントロール解除
		UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
		UDeviceManager* DeviceManager = GameInstance->GetDeviceManager();
		DeviceManager->EnableDefaultActions();
		// チュートリアルを強制的にオフ
		CloseTutorial(Tablet);
		// リセットポイントを変更
		if (UPositionResetComponent* ResetComp = GetComponentByClass<UPositionResetComponent>())
		{
			ResetComp->RecordResetPosition(GetActorLocation());
			ResetComp->RecordResetAngle(GetActorQuat());
		}
		return;
	}

	// 自転車自身の角度の計算
	FRotator Angle = FMath::RInterpTo(Current, _targetRotator, DeltaTime, _rotateSpeed);
	SetActorRelativeRotation(Angle);

	// ハンドルの戻り角度の計算
	HandleBarsAngle = FMath::FInterpTo(HandleBarsAngle, 0.0f, DeltaTime, _handlebarCenteringSpeed);
}

bool ABikeCharacter::CheckOverSpeed() const
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	return Movement->Velocity.Length() >= Movement->GetMaxSpeed();
}

void ABikeCharacter::FindMover()
{
	BikeMovement = GetComponentByClass<UBikeMovementComponent>();
	if (!BikeMovement)
		UE_LOG(LogTemp, Error, TEXT("UBikeMovementComponent didnot attach"));
}

void ABikeCharacter::FindResponder()
{
	Responder = GetComponentByClass<UResponderComponent>();
	if (!Responder)
		UE_LOG(LogTemp, Error, TEXT("UResponderComponent didnot attach"));
}

void ABikeCharacter::FindPhotoCapture()
{
	PhotoCapture = GetComponentByClass<UPhotoCaptureComponent>();
	if (!PhotoCapture)
		UE_LOG(LogTemp, Error, TEXT("UPhotoCaptureComponent didnot attach"));
}

void ABikeCharacter::FindHandStretcher()
{
	LeftHandRoot = FindComponentByTag<UMyHandRootComponent>(LeftHandRootTag);
	if(!LeftHandRoot)
		UE_LOG(LogTemp, Error, TEXT("Left hand root component didnot attach"));

	RightHandRoot = FindComponentByTag<UMyHandRootComponent>(RightHandRootTag);
	if (!RightHandRoot)
		UE_LOG(LogTemp, Error, TEXT("Right hand root component didnot attach"));
}

void ABikeCharacter::FindTabletBaseIcon()
{
	TArray<USceneComponent*> Childs;
	TabletBase->GetChildrenComponents(true, Childs);
	for (USceneComponent* Child : Childs)
		if (UWidgetComponent* Icon = Cast<UWidgetComponent>(Child))
			TabletBaseIcon = Icon;
}

void ABikeCharacter::EnableAutoPlay(AQuestionUIActor* Quiz)
{
	if (BikeMovement)
		BikeMovement->EnableAutoPlay();
	if (Responder)
		Responder->SetQuiz(Quiz);
}

void ABikeCharacter::DisableAutoPlay()
{
	if (BikeMovement)
		BikeMovement->DisableAutoPlay();
	if (Responder)
		Responder->DeleteQuiz();
}

void ABikeCharacter::OpenTutorial(ETutorialType Type)
{
	if (!Tablet)
		return;

	TabletCurrentState = ETabletState::Up;
	Tablet->ShowTutorial(Type);
}

void ABikeCharacter::OnTabletBaseOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATablet* TabletToRetrieve = Cast<ATablet>(OtherActor);
	if (TabletToRetrieve)
	{
		CloseTutorial(TabletToRetrieve);
		TabletCurrentState = ETabletState::Down;
	}
}

bool ABikeCharacter::TakeTablet(UPrimitiveComponent* Hand)
{
	USphereComponent* HandCollision = Cast<USphereComponent>(Hand);
	if (!HandCollision)
		return false;

	FName TagName;
	if (HandCollision->ComponentHasTag(FName("RightHand")))
	{
		TagName = FName("RightTablet");
		Tablet->RotateScene(false);
	}
	else if (HandCollision->ComponentHasTag(FName("LeftHand")))
	{
		TagName = FName("LeftTablet");
		Tablet->RotateScene(true);
	}

	if (TagName.IsNone())
		return false;

	if (USkeletalMeshComponent* HandMesh = Cast<USkeletalMeshComponent>(Hand->GetAttachParent()))
	{
		const USkeletalMeshSocket* HandSocket = HandMesh->GetSocketByName(TagName);
		if (HandSocket)
		{
			HandSocket->AttachActor(Tablet, HandMesh);
			OpenTabletBase();
			return true;
		}
	}
	return false;
}

void ABikeCharacter::MoveUpTablet(float DeltaTime)
{
	if (!Tablet || TabletCurrentState != ETabletState::Up)
		return;

	const USkeletalMeshSocket* TabletSocket = GetMesh()->GetSocketByName(FName("TabletSocket"));
	if (!TabletSocket)
		return;

	// タブレットの世界座標を相対座標に変換
	FVector TabletWorldLocation = Tablet->GetActorLocation();
	FTransform SocketTransform = TabletSocket->GetSocketTransform(GetMesh());
	FVector Current = SocketTransform.InverseTransformPosition(TabletWorldLocation);
	// 移動
	FVector DeltaLocation = FMath::VInterpTo(Current, TabletHighestLocation, DeltaTime, TabletMoveVelocity);
	Tablet->SetActorRelativeLocation(DeltaLocation);

	// 到着
	if (FVector::Dist(DeltaLocation, TabletHighestLocation) < TabletMovementAccuracy)
	{
		// 再配置
		Tablet->SetActorRelativeLocation(TabletHighestLocation);
		TabletCurrentState = ETabletState::Highest;
		// フラッシュの通知エフェクト
		Tablet->FlashForNotify();
		
		Tablet->EnableCollision();
	}
}

void ABikeCharacter::MoveDownTablet(float DeltaTime)
{
	if (!Tablet || TabletCurrentState != ETabletState::Down)
		return;

	const USkeletalMeshSocket* TabletSocket = GetMesh()->GetSocketByName(FName("TabletSocket"));
	if (!TabletSocket)
		return;

	// タブレットの世界座標を相対座標に変換
	FVector TabletWorldLocation = Tablet->GetActorLocation();
	FTransform SocketTransform = TabletSocket->GetSocketTransform(GetMesh());
	FVector Current = SocketTransform.InverseTransformPosition(TabletWorldLocation);
	// 移動
	FVector DeltaLocation = FMath::VInterpTo(Current, TabletLowestLocation, DeltaTime, TabletMoveVelocity);
	Tablet->SetActorRelativeLocation(DeltaLocation);

	if (FVector::Dist(DeltaLocation, TabletLowestLocation) < TabletMovementAccuracy)
	{
		Tablet->SetActorRelativeLocation(TabletLowestLocation);
		TabletCurrentState = ETabletState::Lowest;
		//
		Tablet->Hide();
	}
}

void ABikeCharacter::CloseTutorial(ATablet* TabletToRetrieve)
{
	if (!TabletToRetrieve)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("TabletToRetrieve is null"));
		return;
	}
	const USkeletalMeshSocket* TabletSocket = GetMesh()->GetSocketByName(FName("TabletSocket"));
	if (!TabletSocket)
		return;

	TabletSocket->AttachActor(TabletToRetrieve, GetMesh());
	TabletToRetrieve->SetActorRelativeLocation(TabletHighestLocation);
	TabletToRetrieve->CloseScene();
	TabletToRetrieve->DisableCollision();
	CloseTabletBase();
}

void ABikeCharacter::OpenTabletBase()
{
	if (!TabletBase)
		return;
	TabletBase->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (!TabletBaseIcon)
		return;
	TabletBaseIcon->SetHiddenInGame(false);
}

void ABikeCharacter::CloseTabletBase()
{
	if (!TabletBase)
		return;
	TabletBase->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TabletBase->SetHiddenInGame(true);

	if (!TabletBaseIcon)
		return;
	TabletBaseIcon->SetHiddenInGame(true);
}
