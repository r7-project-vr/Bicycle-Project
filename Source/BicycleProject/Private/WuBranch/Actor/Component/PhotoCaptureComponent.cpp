// Fill out your copyright notice in the Description page of Project Settings.

#include "WuBranch/Actor/Component/PhotoCaptureComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Actor/Animal.h"
#include "Components/WidgetComponent.h"
#include <tokuamaru/IOutlineHighlightable.h>

UPhotoCaptureComponent::UPhotoCaptureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPhotoCaptureComponent::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("Find All ui"));
	FindAllNeedCloseUIs();
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
			SetUIVisibility(false);
			TakePhoto(GameInstance);
			if (TakePhotoSucc)
				UGameplayStatics::PlaySound2D(GetWorld(), TakePhotoSucc);
			DetectAndScoreAnimals();
			SetUIVisibility(true);
		}
		else
		{
			if (TakePhotoFail)
				UGameplayStatics::PlaySound2D(GetWorld(), TakePhotoFail);
		}
	}
}

void UPhotoCaptureComponent::FindAllNeedCloseUIs()
{
	TArray<UActorComponent*> UIs = GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), "UIs");
	for (UActorComponent* UI : UIs)
	{
		if(UI->IsA<UWidgetComponent>())
		{
			UWidgetComponent* WidgetComp = Cast<UWidgetComponent>(UI);
			NeedCloseUIs.Add(WidgetComp);
		}
	}
}

void UPhotoCaptureComponent::SetUIVisibility(bool bVisible)
{
	for (UWidgetComponent* Widget : NeedCloseUIs)
	{
		Widget->SetVisibility(bVisible);
	}
}

void UPhotoCaptureComponent::TakePhoto(UMyGameInstance* GameInstance)
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("World is null!"));
		return;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->PlayerCameraManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController or CameraManager is null!"));
		return;
	}

	// エフェクト
	PC->PlayerCameraManager->StartCameraFade(0.f, 0.8f, 0.1f, FLinearColor::Black);

	FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
	FRotator CameraRotation = PC->PlayerCameraManager->GetCameraRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASceneCapture2D* SceneCapture = GetWorld()->SpawnActor<ASceneCapture2D>(ASceneCapture2D::StaticClass(), CameraLocation, CameraRotation, SpawnParams);

	if (!SceneCapture || !SceneCapture->GetCaptureComponent2D())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create SceneCapture2D!"));
		return;
	}

	int32 Width = 1920;
	int32 Height = 1080;
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
	RenderTarget->InitAutoFormat(Width, Height);
	RenderTarget->UpdateResourceImmediate(true);

	USceneCaptureComponent2D* CaptureComponent = SceneCapture->GetCaptureComponent2D();
	CaptureComponent->TextureTarget = RenderTarget;
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->bCaptureOnMovement = false;

	CaptureComponent->CaptureScene();

	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

	if (!RenderTargetResource)
	{
		UE_LOG(LogTemp, Error, TEXT("RenderTargetResource is null!"));
		SceneCapture->Destroy();
		return;
	}

	TArray<FColor> OutBitmap;
	if (RenderTargetResource->ReadPixels(OutBitmap))
	{
		UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
		if (NewTexture)
		{
#if WITH_EDITORONLY_DATA
			NewTexture->MipGenSettings = TMGS_NoMipmaps;
#endif
			NewTexture->NeverStream = true;
			NewTexture->SRGB = true;

			void* TextureData = NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, OutBitmap.GetData(), OutBitmap.Num() * sizeof(FColor));
			NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
			NewTexture->UpdateResource();

			GameInstance->AddScreenshot(NewTexture);

			UE_LOG(LogTemp, Log, TEXT("Screenshot %d/%d captured! Size: %dx%d"),
				GameInstance->GetScreenshotCount(), GameInstance->GetMaxPhotosPerGame(), Width, Height);

			SceneCapture->Destroy();
			return;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create texture!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read pixels from RenderTarget!"));
	}

	SceneCapture->Destroy();
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