// Fill out your copyright notice in the Description page of Project Settings.

#include "WuBranch/Actor/Component/PhotoCaptureComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Actor/Animal.h"
#include <tokuamaru/IOutlineHighlightable.h>

UPhotoCaptureComponent::UPhotoCaptureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPhotoCaptureComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPhotoCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhotoCaptureBox)
		return;

	// カメラに追従させる
	if (APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()))
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

	TArray<AActor*> OverlappingActors;
	FindCaptureAnimal(OverlappingActors);
	EnableLightAnimal(OverlappingActors);
	DisableLightAnimal(OverlappingActors);
	CapturedAnimals.Empty();
	for (AActor* Actor : OverlappingActors)
	{
		if (AAnimal* Animal = Cast<AAnimal>(Actor))
		{
			CapturedAnimals.Add(Animal);
		}
	}
}

void UPhotoCaptureComponent::InitializePhotoCaptureBox(USceneComponent* AttachParent)
{
	if (!PhotoCaptureBox)
	{
		PhotoCaptureBox = NewObject<UBoxComponent>(GetOwner(), UBoxComponent::StaticClass(), TEXT("PhotoCaptureBox"));
		if (PhotoCaptureBox)
		{
			PhotoCaptureBox->RegisterComponent();
			PhotoCaptureBox->AttachToComponent(AttachParent, FAttachmentTransformRules::KeepRelativeTransform);

			// 動物のみを検出し、他のオブジェクトは無視
			PhotoCaptureBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			PhotoCaptureBox->SetCollisionObjectType(ECC_WorldDynamic);
			PhotoCaptureBox->SetCollisionResponseToAllChannels(ECR_Ignore);
			PhotoCaptureBox->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Overlap);
			PhotoCaptureBox->SetGenerateOverlapEvents(true);

			// サイズ設定
			PhotoCaptureBox->SetBoxExtent(FVector(900.0f, 150.0f, 150.0f));
			PhotoCaptureBox->SetRelativeLocation(FVector(1500.0f, 0.0f, 0.0f));
		}
	}
}

void UPhotoCaptureComponent::OnScreenshotTaken()
{
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
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

void UPhotoCaptureComponent::DetectAndScoreAnimals()
{
	if (!PhotoCaptureBox)
	{
		return;
	}

	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
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

void UPhotoCaptureComponent::FindCaptureAnimal(TArray<AActor*>& OverlappingActors)
{
	if (!PhotoCaptureBox)
		return;

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

void UPhotoCaptureComponent::EnableLightAnimal(TArray<AActor*>& Animals)
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

void UPhotoCaptureComponent::DisableLightAnimal(TArray<AActor*>& Animals)
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

void UPhotoCaptureComponent::DebugPhotoCaptureBox()
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