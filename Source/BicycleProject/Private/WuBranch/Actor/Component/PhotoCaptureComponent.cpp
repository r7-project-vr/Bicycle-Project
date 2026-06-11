// Fill out your copyright notice in the Description page of Project Settings.

#include "WuBranch/Actor/Component/PhotoCaptureComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Actor/WildAnimal.h"
#include "Components/WidgetComponent.h"
#include "WuBranch/Actor/Component/AnimalManagerComponent.h"
#include "WuBranch/Struct/AnimalPhoto.h"

UPhotoCaptureComponent::UPhotoCaptureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPhotoCaptureComponent::BeginPlay()
{
	Super::BeginPlay();
	FindAllNeedCloseUIs();
	AnimalManager = GetOwner()->GetComponentByClass<UAnimalManagerComponent>();
	
}

void UPhotoCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhotoCaptureBox)
		return;

	// カメラに追従させる
	FollowCamera();

	// 2026.05.27 ウー start Tickごとに野良動物を探すの代わりにコリジョンを利用する
	/*TArray<AActor*> OverlappingActors;
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
	}*/
	// 2026.05.27 ウー end
}

void UPhotoCaptureComponent::FollowCamera()
{
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

			// 2026.05.27 ウー start Tickごとに野良動物を探すの代わりにコリジョンを利用する
			// オーバーラップイベントのバインド
			PhotoCaptureBox->OnComponentBeginOverlap.AddDynamic(this, &UPhotoCaptureComponent::OnPhotoCaptureBoxOverlapBegin);
			PhotoCaptureBox->OnComponentEndOverlap.AddDynamic(this, &UPhotoCaptureComponent::OnPhotoCaptureBoxOverlapEnd);
			// 2026.05.27 ウー end
		}
	}
}

void UPhotoCaptureComponent::OnScreenshotTaken()
{
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		// 2026.05.07 ウー start 写真を撮る前に一旦UIを非表示にする
		if (GameInstance->CaptureVRScreenshot())
		{
			SetUIVisibility(false);
			// UIを非表示にしてから写真を撮る、なので同じフレームで動作しない
			FTimerHandle PhotoDelayTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(PhotoDelayTimerHandle, [this, GameInstance]() {
				// 2026.06.10 ウー start 写真を撮るプロセスを改善、名前を取得できるようにする
				TArray<int32> DetectedPetsID = DetectAnimals(GameInstance);
				// 検出した動物を記録する
				for (int32 PetID : DetectedPetsID)
					GameInstance->AddAnimalPhotoPoint(PetID);
				
				TakePhoto(GameInstance, DetectedPetsID);
				if (TakePhotoSuccSE)
					UGameplayStatics::PlaySound2D(GetWorld(), TakePhotoSuccSE);
				//DetectAndScoreAnimals();
				// 2026.06.10 ウー end
				// 写真を撮り終わったらUIを再び表示する
				SetUIVisibility(true);
			}, 0.001f, false);
		}
		// 2026.05.07 ウー end
		else
		{
			if (TakePhotoFailSE)
				UGameplayStatics::PlaySound2D(GetWorld(), TakePhotoFailSE);
		}
	}
}

// 2026.05.27 ウー start Tickごとに野良動物を探すの代わりにコリジョンを利用する
void UPhotoCaptureComponent::OnPhotoCaptureBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (AWildAnimal* WildAnimal = Cast<AWildAnimal>(OtherActor))
		{
			// 野良動物が範囲に入ったとき、ハイライトを有効にする
			IIOutlineHighlightable::Execute_EnableHighlight(WildAnimal);
			CapturedAnimals.Add(WildAnimal);
		}
	}
}

void UPhotoCaptureComponent::OnPhotoCaptureBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (AWildAnimal* WildAnimal = Cast<AWildAnimal>(OtherActor))
		{
			// 野良動物が範囲から出たとき、ハイライトを無効にする
			IIOutlineHighlightable::Execute_DisableHighlight(WildAnimal);
			CapturedAnimals.Remove(WildAnimal);
		}
	}
}
// 2026.05.27 ウー end

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
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Found UI Num: %d"), NeedCloseUIs.Num()));
}

void UPhotoCaptureComponent::SetUIVisibility(bool bVisible)
{
	for (UWidgetComponent* Widget : NeedCloseUIs)
	{
		Widget->SetVisibility(bVisible);
	}
}

void UPhotoCaptureComponent::TakePhoto(UMyGameInstance* GameInstance, TArray<int32> DetectedPetsID)
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
	// 2026.05.07 ウー start
	// SCS_FinalColorLDRだと暗い部分はあまり見えないので、SCS_SceneColorHDRに変更
	//CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	// 2026.05.07 ウー end
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

			FAnimalPhoto AnimalPhoto = MakePhotoDatas(DetectedPetsID, NewTexture);
			if(AnimalPhoto.Photo)
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("has photo"));
			GameInstance->AddScreenshot(AnimalPhoto);

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

//void UPhotoCaptureComponent::DetectAndScoreAnimals()
//{
//	if (!PhotoCaptureBox)
//	{
//		return;
//	}
//
//	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
//	if (!GameInstance)
//	{
//		return;
//	}
//
//	// 2026.05.27 ウー start Tickごとに野良動物を探すの代わりにコリジョンを利用する
//	//TArray<AActor*> OverlappingActors;
//	//FindCaptureAnimal(OverlappingActors);
//
//	TSet<int32> DetectedAnimalIDs;
//
//	// 直接CapturedAnimalsからIDを取得する
//	for (TWeakObjectPtr<AWildAnimal> Animal : CapturedAnimals)
//	{
//		if (AWildAnimal* WildAnimal = Animal.Get())
//		{
//			// 一枚の写真に複数の動物が映った場合、全部の動物を計算に入れます
//			int32 PetID = GameInstance->SwitchWild2Pet(WildAnimal->GetMyID());
//			DetectedAnimalIDs.Add(PetID);
//			GameInstance->AddAnimalPhotoPoint(PetID);
//		}
//	}
//
//	/*for (AActor* Actor : OverlappingActors)
//	{
//		AAnimal* Animal = Cast<AAnimal>(Actor);
//		if (Animal)
//		{
//			if (Animal->Implements<UIOutlineHighlightable>())
//			{
//				int32 AnimalID = Animal->GetMyID();
//				if (!DetectedAnimalIDs.Contains(AnimalID))
//				{
//					DetectedAnimalIDs.Add(AnimalID);
//					int32 PetID = GameInstance->SwitchWild2Pet(AnimalID);
//					GameInstance->AddAnimalPhotoPoint(PetID);
//				}
//			}
//		}
//	}*/
//	// 2026.05.27 ウー end
//}

TArray<int32> UPhotoCaptureComponent::DetectAnimals(UMyGameInstance* GameInstance)
{
	TArray<int32> DetectedAnimalIDs;

	// 直接CapturedAnimalsからIDを取得する
	for (TWeakObjectPtr<AWildAnimal> Animal : CapturedAnimals)
	{
		if (AWildAnimal* WildAnimal = Animal.Get())
		{
			// 一枚の写真に複数の動物が映った場合、全部の動物を計算に入れます
			int32 PetID = GameInstance->SwitchWild2Pet(WildAnimal->GetMyID());
			DetectedAnimalIDs.Add(PetID);
		}
	}

	return DetectedAnimalIDs;
}

FAnimalPhoto UPhotoCaptureComponent::MakePhotoDatas(TArray<int32> DetectedPetsID, UTexture2D* Texture)
{
	FAnimalPhoto Photo;

	if (!AnimalManager)
		return Photo;

	TArray<FName> AnimalNames;
	for (int32 PetID : DetectedPetsID)
	{
		FName Name = AnimalManager->GetAnimalName(PetID);
		if (!Name.IsNone())
		{
			AnimalNames.Add(Name);
		}
	}

	Photo.Photo = Texture;
	Photo.Names = AnimalNames;
	Photo.bIsRare = false;

	return Photo;
}

//void UPhotoCaptureComponent::FindCaptureAnimal(TArray<AActor*>& OverlappingActors)
//{
//	if (!PhotoCaptureBox)
//		return;
//
//	// 2026.05.27 ウー start Tickごとに野良動物を探すの代わりにコリジョンを利用する
//	/*TArray<AActor*> TempOverlappingActors;
//	PhotoCaptureBox->GetOverlappingActors(TempOverlappingActors, AAnimal::StaticClass());*/
//
//	if (CapturedAnimals.Num() == 0)
//	{
//		TArray<AActor*> AllAnimals;
//		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWildAnimal::StaticClass(), AllAnimals);
//
//		FVector BoxLocation = PhotoCaptureBox->GetComponentLocation();
//		FVector BoxExtent = PhotoCaptureBox->GetScaledBoxExtent();
//		FRotator BoxRotation = PhotoCaptureBox->GetComponentRotation();
//
//		for (AActor* Actor : AllAnimals)
//		{
//			AWildAnimal* Animal = Cast<AWildAnimal>(Actor);
//			if (Animal)
//			{
//				FVector AnimalLocation = Animal->GetActorLocation();
//				FVector LocalPos = BoxRotation.UnrotateVector(AnimalLocation - BoxLocation);
//
//				bool bInRange = (FMath::Abs(LocalPos.X) <= BoxExtent.X) &&
//					(FMath::Abs(LocalPos.Y) <= BoxExtent.Y) &&
//					(FMath::Abs(LocalPos.Z) <= BoxExtent.Z);
//
//				if (bInRange)
//				{
//					OverlappingActors.Add(Animal);
//				}
//			}
//		}
//	}
//	// 2026.05.27 ウー end
//}

//void UPhotoCaptureComponent::EnableLightAnimal(TArray<AActor*>& Animals)
//{
//	for (AActor* Actor : Animals)
//	{
//		if (AAnimal* Animal = Cast<AAnimal>(Actor))
//		{
//			if (Animal->Implements<UIOutlineHighlightable>())
//			{
//				IIOutlineHighlightable::Execute_EnableHighlight(Animal);
//			}
//		}
//	}
//}

//void UPhotoCaptureComponent::DisableLightAnimal(TArray<AActor*>& Animals)
//{
//	TArray<AWildAnimal*> LeftAnimals;
//	for (const TWeakObjectPtr<AWildAnimal>& PrevAnimal : CapturedAnimals)
//	{
//		if (!PrevAnimal.IsValid())
//		{
//			continue; // Destroyされたもの
//		}
//
//		if (!Animals.Contains(PrevAnimal))
//		{
//			LeftAnimals.Add(PrevAnimal.Get());
//		}
//	}
//
//	for (AWildAnimal* Animal : LeftAnimals)
//	{
//		IIOutlineHighlightable::Execute_DisableHighlight(Animal);
//	}
//}

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