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
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/Actor/Animal.h"
#include <tokuamaru/IOutlineHighlightable.h>
#include <WuBranch/UI/QuestionUIActor.h>

// Sets default values
ABikeCharacter::ABikeCharacter()
	: IsOverSpeed(false)
	, IsPause(false)
{
	PrimaryActorTick.bCanEverTick = true;

	Bike = CreateDefaultSubobject<UBikeComponent>(TEXT("Bike"));
	AnimalManager = CreateDefaultSubobject<UAnimalManagerComponent>(TEXT("Animal Manager"));
	PhotoCapture = CreateDefaultSubobject<UPhotoCaptureComponent>(TEXT("Photo Capture"));

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

	FindMover();
	FindResponder();
	_widgetInteractionHeadComponent = GetComponentByClass<UWidgetInteractionHeadComponent>();
	LoadBikeMesh();
	_targetRotator = FRotator::ZeroRotator;
	IsOverSpeed = false;
	IsPause = false;

	// PhotoCaptureBoxを初期化
	if (PhotoCapture)
	{
		PhotoCapture->InitializePhotoCaptureBox(RootComponent);
	}

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
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Bind event")));
		}
		if (!Responder)
			FindResponder();
		if (Responder)
		{
			DeviceManager->BindSelectLeftEvent(Responder, "OnSelectLeftAnswer");
			DeviceManager->BindSelectRightEvent(Responder, "OnSelectRightAnswer");
		}

		// PhotoCaptureComponentで撮影を行う
		if (PhotoCapture)
		{
			DeviceManager->BindScreenshotEvent(PhotoCapture, FName("OnScreenshotTaken"));
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
	// 移動の機能をBikeMovementに移動したため
	//Bike->ReduceVelocityTo0();
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
		// 強制コントロール解除、その前にゲームオーバーしたかどうかを確認する
		//if (!Cast<AQuestionGameMode>(GetWorld()->GetAuthGameMode())->IsGameFailed())
		//{
			UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
			UDeviceManager* DeviceManager = GameInstance->GetDeviceManager();
			DeviceManager->EnableDefaultActions();
		//}
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
		UE_LOG(LogTemp, Error, TEXT("UBikeMovementComponent didnot attach"));
}

void ABikeCharacter::OnScreenshotTaken()
{
	UMyGameInstance* GameInstance = GetGameInstance<UMyGameInstance>();
	if (GameInstance)
	{
		if (GameInstance->CaptureVRScreenshot())
		{
			if (TakePhotoSucc)
				UGameplayStatics::PlaySound2D(GetWorld(), TakePhotoSucc);
			DetectAndScoreAnimals();
		}
		else
		{
			if (TakePhotoFail)
				UGameplayStatics::PlaySound2D(GetWorld(), TakePhotoFail);
		}
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

	TArray<AActor*> OverlappingActors;
	FindCaptureAnimal(OverlappingActors);

	TSet<int32> DetectedAnimalIDs;

	for (AActor* Actor : OverlappingActors)
	{
		AAnimal* Animal = Cast<AAnimal>(Actor);
		if (Animal)
		{
			if (Animal->Implements<UIOutlineHighlightable>())
			{
				int32 AnimalID = Animal->GetMyID();
				if (!DetectedAnimalIDs.Contains(AnimalID))
				{
					DetectedAnimalIDs.Add(AnimalID);
					int32 PetID = GameInstance->SwitchWild2Pet(AnimalID);
					GameInstance->AddAnimalPhotoPoint(PetID);
				}
			}
		}
	}
}

void ABikeCharacter::FindCaptureAnimal(TArray<AActor*>& OverlappingActors)
{
	PhotoCaptureBox->GetOverlappingActors(OverlappingActors, AAnimal::StaticClass());

	if (OverlappingActors.Num() == 0)
	{
		TArray<AActor*> AllAnimals;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAnimal::StaticClass(), AllAnimals);

		FVector BoxLocation = PhotoCaptureBox->GetComponentLocation();
		FVector BoxExtent = PhotoCaptureBox->GetScaledBoxExtent();
		FRotator BoxRotation = PhotoCaptureBox->GetComponentRotation();

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
}

void ABikeCharacter::EnableLightAnimal(TArray<AActor*>& Animals)
{
	for (AActor* Actor : Animals)
	{
		if (AAnimal* Animal = Cast<AAnimal>(Actor))
		{
			if (Animal->Implements<UIOutlineHighlightable>())
			{
				IIOutlineHighlightable::Execute_EnableHighlight(Animal);
			}
		}
	}
}

void ABikeCharacter::DisableLightAnimal(TArray<AActor*>& Animals)
{
	TArray<AAnimal*> LeftAnimals;
	for (const TWeakObjectPtr<AAnimal>& PrevAnimal : CapturedAnimals)
	{
		if (!PrevAnimal.IsValid())
		{
			continue; // Destroyされたもの
		}

		if (!Animals.Contains(PrevAnimal))
		{
			LeftAnimals.Add(PrevAnimal.Get());
		}
	}

	for (AAnimal* Animal : LeftAnimals)
	{
		if (Animal->Implements<UIOutlineHighlightable>())
		{
			IIOutlineHighlightable::Execute_DisableHighlight(Animal);
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
