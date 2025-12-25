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
#include "Components/BoxComponent.h"
#include "WuBranch/Actor/Animal.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABikeCharacter::ABikeCharacter()
	: IsOverSpeed(false)
	, IsPause(false)
{
	PrimaryActorTick.bCanEverTick = true;

	Bike = CreateDefaultSubobject<UBikeComponent>(FName("Bike"));
	AnimalManager = CreateDefaultSubobject<UAnimalManagerComponent>(TEXT("Animal Manager"));

	// 撮影判定用コリジョンを作成
	PhotoCaptureBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PhotoCaptureBox"));
	PhotoCaptureBox->SetupAttachment(RootComponent);
	
	// コリジョン設定 - 動物のみを検出し、他のオブジェクトは無視
	PhotoCaptureBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PhotoCaptureBox->SetCollisionObjectType(ECC_WorldDynamic);
	PhotoCaptureBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	PhotoCaptureBox->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Overlap);
	PhotoCaptureBox->SetGenerateOverlapEvents(true);
	
	// サイズ設定
	PhotoCaptureBox->SetBoxExtent(FVector(900.0f, 150.0f, 150.0f));
	PhotoCaptureBox->SetRelativeLocation(FVector(1500.0f, 0.0f, 0.0f));
	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	_handlebarCenteringSpeed = 1.0f;
	_isRotate = false;
	HandleBarsAngle = 0.0f;
}

// Called when the game starts or when spawned
void ABikeCharacter::BeginPlay()
{
	Super::BeginPlay();

	_widgetInteractionHeadComponent = GetComponentByClass<UWidgetInteractionHeadComponent>();
	LoadBikeMesh();
	_targetRotator = FRotator::ZeroRotator;
	IsOverSpeed = false;
	IsPause = false;

	// ゲーム開始時にスクリーンショットをリセット
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		GameInstance->ResetScreenshots();
	}
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

	// カメラに追従させる
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (PC->PlayerCameraManager)
		{
			FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
			FRotator CameraRotation = PC->PlayerCameraManager->GetCameraRotation();
			
			// カメラの前方に判定ボックスを配置
			FVector ForwardVector = CameraRotation.Vector();
			FVector BoxLocation = CameraLocation + (ForwardVector * 1500.0f);
			
			PhotoCaptureBox->SetWorldLocation(BoxLocation);
			PhotoCaptureBox->SetWorldRotation(CameraRotation);
		}
	}
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
		UDeviceManager* DeviceManager = GameInstance->GetDeviceManager();
		DeviceManager->CreateAllDevices();
		DeviceManager->BindMoveEvent(Bike, "OnMove");
		DeviceManager->BindSelectLeftEvent(Bike, "OnSelectLeftAnswer");
		DeviceManager->BindSelectRightEvent(Bike, "OnSelectRightAnswer");
		/*if(BikeMovement)
			DeviceManager->BindMoveEvent(BikeMovement, "OnMove");
		if (Responder)
		{
			DeviceManager->BindSelectLeftEvent(Responder, "OnSelectLeftAnswer");
			DeviceManager->BindSelectRightEvent(Responder, "OnSelectRightAnswer");
		}*/
		

		// BikeCharacterで撮影を行う
		DeviceManager->BindScreenshotEvent(this, FName("OnScreenshotTaken"));
	}
}

void ABikeCharacter::Pause_Implementation()
{
	IsPause = true;
	if (!Bike->GetIsAutoPlay())
	{
		UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
		if (GameInstance)
		{
			if (UDeviceManager* DeviceManager = GameInstance->GetDeviceManager())
			{
				DeviceManager->DisableDefaultActions();

				if (Bike->GetIsAutoPlay())
					DeviceManager->DisableSelectAnswerActions();
			}
		}
		StopMove();
	}
}

void ABikeCharacter::ReStart_Implementation()
{
	IsPause = false;
	if (!Bike->GetIsAutoPlay())
	{
		UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
		if (GameInstance)
		{
			if (UDeviceManager* DeviceManager = GameInstance->GetDeviceManager())
			{
				DeviceManager->EnableDefaultActions();

				if (Bike->GetIsAutoPlay())
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

void ABikeCharacter::SetTurningAngle(FRotator angle)
{
	_targetRotator = GetActorRotation() + angle;
	// 右折
	if (angle.Yaw > 0)
		HandleBarsAngle = -30.0f;
	// 左折
	else if (angle.Yaw < 0)
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
	Bike->ReduceVelocityTo0();
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
		const FSoftObjectPath& assetRef = _bikeSkeletalNeedLoad.ToSoftObjectPath();
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(assetRef, FStreamableDelegate::CreateUObject(this, &ABikeCharacter::LoadMeshComplete));
	}
}

void ABikeCharacter::LoadMeshComplete()
{
	USkeletalMesh* skeletalMesh = _bikeSkeletalNeedLoad.Get();
	if (skeletalMesh)
	{
		GetMesh()->SetSkeletalMesh(skeletalMesh);
	}
}

void ABikeCharacter::RotateBike(float DeltaTime)
{
	if (!_isRotate)
		return;

	FRotator current = GetActorRotation();
	// 曲がった
	if (current.Equals(_targetRotator, 0.5f))
	{
		// 0.5度未満の時は曲がり終了と見なすため、強制的に角度を最終角度に設定
		SetActorRelativeRotation(_targetRotator);
		HandleBarsAngle = 0.0f;
		_isRotate = false;
		// 強制コントロール解除、その前にゲームオーバーしたかどうかを確認する
		//if (!Cast<AQuestionGameMode>(GetWorld()->GetAuthGameMode())->IsGameFailed())
		//{
			UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
			UDeviceManager* deviceManager = gameInstance->GetDeviceManager();
			deviceManager->EnableDefaultActions();
		//}
		return;
	}

	// 自転車自身の角度の計算
	FRotator angle = FMath::RInterpTo(current, _targetRotator, DeltaTime, _rotateSpeed);
	SetActorRelativeRotation(angle);

	// ハンドルの戻り角度の計算
	HandleBarsAngle = FMath::FInterpTo(HandleBarsAngle, 0.0f, DeltaTime, _handlebarCenteringSpeed);
}

bool ABikeCharacter::CheckOverSpeed() const
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	return Movement->Velocity.Length() >= Movement->GetMaxSpeed();
}

void ABikeCharacter::OnScreenshotTaken()
{
	UMyGameInstance* GameInstance = GetGameInstance<UMyGameInstance>();
	if (GameInstance)
	{
		GameInstance->CaptureVRScreenshot();
		DetectAndScoreAnimals();
	}
}

void ABikeCharacter::DetectAndScoreAnimals()
{
	if (!PhotoCaptureBox)
	{
		return;
	}

	UMyGameInstance* GameInstance = GetGameInstance<UMyGameInstance>();
	if (!GameInstance)
	{
		return;
	}

	FVector BoxLocation = PhotoCaptureBox->GetComponentLocation();
	FVector BoxExtent = PhotoCaptureBox->GetScaledBoxExtent();
	FRotator BoxRotation = PhotoCaptureBox->GetComponentRotation();

	TArray<AActor*> AllAnimals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAnimal::StaticClass(), AllAnimals);

	TArray<AActor*> OverlappingActors;
	PhotoCaptureBox->GetOverlappingActors(OverlappingActors, AAnimal::StaticClass());

	if (OverlappingActors.Num() == 0)
	{
		for (AActor* Actor : AllAnimals)
		{
			AAnimal* Animal = Cast<AAnimal>(Actor);
			if (Animal)
			{
				FVector AnimalLocation = Animal->GetActorLocation();
				FVector LocalPos = BoxRotation.UnrotateVector(AnimalLocation - BoxLocation);
				
				bool bInRange = (FMath::Abs(LocalPos.X) <= BoxExtent.X) &&
				                (FMath::Abs(LocalPos.Y) <= BoxExtent.Y) &&
				                (FMath::Abs(LocalPos.Z) <= BoxExtent.Z);
				
				if (bInRange)
				{
					OverlappingActors.Add(Animal);
				}
			}
		}
	}

	TSet<int32> DetectedAnimalIDs;

	for (AActor* Actor : OverlappingActors)
	{
		AAnimal* Animal = Cast<AAnimal>(Actor);
		if (Animal)
		{
			int32 AnimalID = Animal->GetMyID();
			
			if (!DetectedAnimalIDs.Contains(AnimalID))
			{
				DetectedAnimalIDs.Add(AnimalID);
				GameInstance->AddAnimalPhotoPoint(AnimalID);
			}
		}
	}
}

void ABikeCharacter::DebugPhotoCaptureBox()
{
	if (!PhotoCaptureBox)
	{
		UE_LOG(LogTemp, Error, TEXT("PhotoCaptureBox is null!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("=== PhotoCaptureBox Debug Info ==="));
	UE_LOG(LogTemp, Warning, TEXT("Collision Enabled: %d"), (int32)PhotoCaptureBox->GetCollisionEnabled());
	UE_LOG(LogTemp, Warning, TEXT("Generate Overlap Events: %s"), PhotoCaptureBox->GetGenerateOverlapEvents() ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("Box Extent: %s"), *PhotoCaptureBox->GetScaledBoxExtent().ToString());
	
	// 全てのコリジョンレスポンスを表示
	UE_LOG(LogTemp, Warning, TEXT("Collision Responses:"));
	UE_LOG(LogTemp, Warning, TEXT("  Pawn: %d"), (int32)PhotoCaptureBox->GetCollisionResponseToChannel(ECC_Pawn));
	UE_LOG(LogTemp, Warning, TEXT("  WorldDynamic: %d"), (int32)PhotoCaptureBox->GetCollisionResponseToChannel(ECC_WorldDynamic));
}
