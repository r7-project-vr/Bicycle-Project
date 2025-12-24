// Fill out your copyright notice in the Description page of Project Settings.

#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Device/DeviceManager.h"
#include <UntakuBranch/Question.h>
#include "WuBranch/Actor/Animal.h"
#include <Kismet/GameplayStatics.h>
#include "WuBranch/Struct/PlayerSaveGame.h"
#include "WuBranch/System/SaveGameManager.h"
#include "Engine/Texture2D.h"
#include "Engine/GameViewportClient.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealClient.h"
#include "NarisawaBranch/ScreenshotDisplayActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/SceneCapture2D.h"
#include "WuBranch/Struct/ShopItem.h"

UMyGameInstance::UMyGameInstance()
	: TotalCoins(0)
	, CoinsPerGame(0)
	, CoinHeight(475.f)
	, IsClear(false)
	, StandardRPM(50)
	, RPMThreshold(10)
	, MaxStandardRPM(60)
	, NumOfSets(4)
	, MaxAnimalCount(10)
	, ShopItemsDataTable(nullptr)
{
	DeviceManager = nullptr;
	FileName = TEXT("PlayerData.json");
}

void UMyGameInstance::Init()
{
	Super::Init();

	if (!DeviceManager)
	{
		DeviceManager = NewObject<UDeviceManager>(this);
	}

	GetSubsystem<USaveGameManager>()->OnLoadCompleted.AddDynamic(this, &UMyGameInstance::OnLoadComplete);
	GetSubsystem<USaveGameManager>()->OnSaveCompleted.AddDynamic(this, &UMyGameInstance::OnSaveComplete);

	ReadAll();
}

#pragma region デバイス
UDeviceManager* UMyGameInstance::GetDeviceManager() const
{
	return DeviceManager;
}
#pragma endregion

#pragma region コイン
int UMyGameInstance::GetTotalCoins() const
{
	return TotalCoins;
}

void UMyGameInstance::SetTotalCoins(int Amount)
{
	TotalCoins = FMath::Clamp(Amount, 0, INT32_MAX);
	NotifyUpdateCoin();
}

void UMyGameInstance::AddCoinsPerGame(int Amount)
{
	// 無効な値
	if(Amount <= 0)
		return;

	CoinsPerGame += Amount;
	NotifyUpdateCoinPerGame();
}

void UMyGameInstance::ResetCoinsPerGame()
{
	CoinsPerGame = 0;
	NotifyUpdateCoinPerGame();
}

int UMyGameInstance::GetCoinsPerGame() const
{
	return CoinsPerGame;
}

float UMyGameInstance::GetCoinHeight() const
{
	return CoinHeight;
}

void UMyGameInstance::SetCoinHeight(float Height)
{
	CoinHeight = Height;
	NotifyUpdateCoinHeight();
}

void UMyGameInstance::ResetCoinHeight()
{
	CoinHeight = 100.f;
	NotifyUpdateCoinHeight();
}

void UMyGameInstance::SaveCoinsToFile(FPlayerSaveGame& Data)
{
	Data.Coins = TotalCoins;
}

void UMyGameInstance::ReadCoinFromFile(const FPlayerSaveGame& Data)
{
	TotalCoins = Data.Coins;
	CoinHeight = 100.f;
	NotifyUpdateCoinHeight();
}

void UMyGameInstance::NotifyUpdateCoin()
{
	if (OnUpdateCoin.IsBound())
	{
		OnUpdateCoin.Broadcast(TotalCoins);
	}
}

void UMyGameInstance::NotifyUpdateCoinPerGame()
{
	if (OnUpdateCoinPerGame.IsBound())
	{
		OnUpdateCoinPerGame.Broadcast(CoinsPerGame);
	}
}

void UMyGameInstance::NotifyUpdateCoinHeight()
{
	if (OnUpdateCoinHeight.IsBound())
	{
		OnUpdateCoinHeight.Broadcast(CoinHeight);
	}
}
#pragma endregion

#pragma region ゲーム結果
void UMyGameInstance::SaveQuizsForResult(TArray<FQuestion*> Result)
{
	Quizs.Empty();
	for (FQuestion* Quiz : Result)
	{
		Quizs.Add(*Quiz);
	}
}

void UMyGameInstance::SetGameResult(bool bResult)
{
	IsClear = bResult;
}
#pragma endregion

#pragma region RPM
void UMyGameInstance::SetStandardRPM(int Value)
{
	StandardRPM = FMath::Clamp(Value, 0, MaxStandardRPM);
	NotifyUpdateRPM();
}

int UMyGameInstance::GetStandardRPM() const
{
	return StandardRPM;
}

void UMyGameInstance::ResetStandardRPM()
{
	StandardRPM = 50;
	NotifyUpdateRPM();
}

void UMyGameInstance::AdjustThreshold(int Value)
{
	RPMThreshold += Value;
	NotifyUpdateRPM();
}

int UMyGameInstance::GetThreshold() const
{
	return RPMThreshold;
}

void UMyGameInstance::ResetThreshold()
{
	RPMThreshold = 10;
	NotifyUpdateRPM();
}

int UMyGameInstance::GetDangerRPM() const
{
	return StandardRPM + RPMThreshold;
}

int UMyGameInstance::GetSafeRPM() const
{
	return StandardRPM - RPMThreshold;
}

int UMyGameInstance::GetMaxRPM() const
{
	return MaxStandardRPM;
}

void UMyGameInstance::NotifyUpdateRPM()
{
	if (OnUpdateRPM.IsBound())
		OnUpdateRPM.Broadcast(GetStandardRPM(), GetDangerRPM(), GetSafeRPM());
}

void UMyGameInstance::SaveRPMToFile(FPlayerSaveGame& Data)
{
	Data.RPMLimit = StandardRPM;
	Data.RPMThreshold = RPMThreshold;
}

void UMyGameInstance::ReadRPMFromFile(const FPlayerSaveGame& Data)
{
	StandardRPM = Data.RPMLimit;
	RPMThreshold = Data.RPMThreshold;
	MaxStandardRPM = 60;
	NotifyUpdateRPM();
}
#pragma endregion

#pragma region セット数
void UMyGameInstance::SetNumOfSets(int Value)
{
	NumOfSets = Value;
}

int32 UMyGameInstance::GetNumOfSets() const
{
	return NumOfSets;
}

void UMyGameInstance::SaveSetsToFile(FPlayerSaveGame& Data)
{
	Data.MapSets = NumOfSets;
}

void UMyGameInstance::ReadSetsFromFile(const FPlayerSaveGame& Data)
{
	NumOfSets = Data.MapSets;
}
#pragma endregion

#pragma region 動物
void UMyGameInstance::AddAnimal(int32 AnimalID)
{
	// もう入れられない
	if (HasMaxAnimals())
		return;

	// 追加
	if (OwnedAnimals.Contains(AnimalID))
		OwnedAnimals[AnimalID] += 1;
	else
		OwnedAnimals.Add(AnimalID, 1);

	// 通知
	NotifyUpdateAnimalNum(AnimalID);
}

void UMyGameInstance::RemoveAnimal(int32 AnimalID)
{
	// 配列のなかに見当たらない
	if (!OwnedAnimals.Contains(AnimalID))
		return;

	// 削除
	OwnedAnimals[AnimalID] -= 1;

	if(OwnedAnimals[AnimalID] == 0)
		OwnedAnimals.Remove(AnimalID);

	// 通知
	NotifyUpdateAnimalNum(AnimalID);
}

TArray<TSubclassOf<AAnimal>> UMyGameInstance::GetAnimals() const
{
	return Animals;
}

TMap<int32, int32> UMyGameInstance::GetOwnedAnimals() const
{
	return OwnedAnimals;
}

int UMyGameInstance::GetAnimalNumByID(int32 AnimalID) const
{
	if (!OwnedAnimals.Contains(AnimalID))
		return 0;

	return OwnedAnimals[AnimalID];
}

void UMyGameInstance::SetMaxAnimalCount(int Amount)
{
	MaxAnimalCount = Amount;
}

bool UMyGameInstance::HasMaxAnimals() const
{
	int Sum = 0;
		for (const auto& Pair : OwnedAnimals)
	{
		Sum += Pair.Value;
	}
	return Sum >= MaxAnimalCount;
}

void UMyGameInstance::NotifyUpdateAnimalNum(int32 AnimalID)
{
	if (OnUpdateAnimalNum.IsBound())
	{
		int Nums = GetAnimalNumByID(AnimalID);
		OnUpdateAnimalNum.Broadcast(AnimalID, Nums);
	}
}

void UMyGameInstance::NotifyUpdateAnimalNum(int32 AnimalID, int Nums)
{
	if (OnUpdateAnimalNum.IsBound())
	{
		OnUpdateAnimalNum.Broadcast(AnimalID, Nums);
	}
}

void UMyGameInstance::SaveAnimalToFile(FPlayerSaveGame& Data)
{
	Data.OwnedAnimals = OwnedAnimals;
}

void UMyGameInstance::ReadAnimalFromFile(const FPlayerSaveGame& Data)
{
	OwnedAnimals = Data.OwnedAnimals;
}
#pragma endregion

#pragma region 写真
void UMyGameInstance::AddAnimalPhoto(int32 AnimalID, int32 Nums)
{
	if (AnimalPhotoNums.Contains(AnimalID))
		AnimalPhotoNums[AnimalID] += Nums;
	else
		AnimalPhotoNums[AnimalID] = Nums;
}

int UMyGameInstance::GetAnimalPhotoNum(int32 AnimalID) const
{
	if (AnimalPhotoNums.Contains(AnimalID))
		return AnimalPhotoNums[AnimalID];
	else
		return 0;
}

void UMyGameInstance::ResetAnimalPhoto()
{
	AnimalPhotoNums.Empty();
}

void UMyGameInstance::AddAnimalPhotoPoint(int32 AnimalID)
{
	// 既存のポイント数を記録
	int32 OldPoints = AnimalPhotoPoints.Contains(AnimalID) ? AnimalPhotoPoints[AnimalID] : 0;
	
	if (AnimalPhotoPoints.Contains(AnimalID))
		AnimalPhotoPoints[AnimalID] += 1;
	else
		AnimalPhotoPoints.Add(AnimalID, 1);
}

int32 UMyGameInstance::GetAnimalPhotoPoint(int32 AnimalID) const
{
	if (AnimalPhotoPoints.Contains(AnimalID))
		return AnimalPhotoPoints[AnimalID];
	else
		return 0;
}

int32 UMyGameInstance::GetRequiredPointsForAnimal(int32 AnimalID) const
{
	if (!ShopItemsDataTable)
	{
		return 3; // デフォルト値
	}

	TArray<FShopItem*> AllShopItems;
	FString ContextString(TEXT("GetRequiredPointsForAnimal"));
	ShopItemsDataTable->GetAllRows<FShopItem>(ContextString, AllShopItems);
	
	for (FShopItem* ShopItem : AllShopItems)
	{
		if (ShopItem && ShopItem->ID == AnimalID)
		{
			return ShopItem->UnLockLimit;
		}
	}

	return 3; // デフォルト値
}

bool UMyGameInstance::CanPurchaseAnimal(int32 AnimalID) const
{
	if (HasMaxAnimals())
	{
		return false;
	}

	int32 CurrentPoints = GetAnimalPhotoPoint(AnimalID);
	int32 RequiredPoints = GetRequiredPointsForAnimal(AnimalID);
	
	return CurrentPoints >= RequiredPoints;
}

bool UMyGameInstance::PurchaseAnimal(int32 AnimalID)
{
	if (!CanPurchaseAnimal(AnimalID))
	{
		return false;
	}

	AddAnimal(AnimalID);
	
	return true;
}

bool UMyGameInstance::IsAnimalOwned(int32 AnimalID) const
{
	return OwnedAnimals.Contains(AnimalID) && OwnedAnimals[AnimalID] > 0;
}

int32 UMyGameInstance::GetTotalPhotoPoints() const
{
	int32 Total = 0;
	for (const auto& Pair : AnimalPhotoPoints)
	{
		Total += Pair.Value;
	}
	return Total;
}

void UMyGameInstance::ResetPhotoPoints()
{
	AnimalPhotoPoints.Empty();
}

void UMyGameInstance::SavePhotoToFile(FPlayerSaveGame& Data)
{
	Data.AnimalPhotos = AnimalPhotoNums;
	Data.AnimalPhotoPoints = AnimalPhotoPoints;
}

void UMyGameInstance::ReadPhotoFromFile(const FPlayerSaveGame& Data)
{
	AnimalPhotoNums = Data.AnimalPhotos;
	AnimalPhotoPoints = Data.AnimalPhotoPoints;
}
#pragma endregion

#pragma region 自転車調整
void UMyGameInstance::SetBikeOffset(FVector Offset)
{
	BikeOffset = Offset;
}

FVector UMyGameInstance::GetBikeOffset() const
{
	return BikeOffset;
}
#pragma endregion

#pragma region セーブ
void UMyGameInstance::SaveAllToFile()
{
	FPlayerSaveGame Data;
	SaveAnimalToFile(Data);
	SaveCoinsToFile(Data);
	SaveSetsToFile(Data);
	SaveRPMToFile(Data);
	SavePhotoToFile(Data);
	
	FString Path = FPaths::ProjectSavedDir() / FileName;
	GetSubsystem<USaveGameManager>()->SaveFile(Path, Data);
}

void UMyGameInstance::ReadAll()
{
	FString Path = FPaths::ProjectSavedDir() / FileName;
	GetSubsystem<USaveGameManager>()->LoadFile(Path);
}

void UMyGameInstance::OnSaveComplete(bool bResult)
{
	if (!bResult)
	{
		UE_LOG(LogTemp, Error, TEXT("Save file error"));
	}
}

void UMyGameInstance::OnLoadComplete(const FPlayerSaveGame& Data)
{
	ReadCoinFromFile(Data);
	ReadRPMFromFile(Data);
	ReadSetsFromFile(Data);
	ReadAnimalFromFile(Data);
	ReadPhotoFromFile(Data);
}
#pragma endregion

#pragma region スクリーンショット
void UMyGameInstance::CaptureVRScreenshot()
{
	if (CapturedScreenshots.Num() >= MaxScreenshotsPerGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("Maximum screenshots reached (%d/%d). Cannot take more screenshots this game."), 
			CapturedScreenshots.Num(), MaxScreenshotsPerGame);
		return;
	}

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
			
			CapturedScreenshots.Add(NewTexture);
			
			UE_LOG(LogTemp, Log, TEXT("Screenshot %d/%d captured! Size: %dx%d"), 
				CapturedScreenshots.Num(), MaxScreenshotsPerGame, Width, Height);
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

int32 UMyGameInstance::GetRemainingScreenshots() const
{
	return MaxScreenshotsPerGame - CapturedScreenshots.Num();
}

void UMyGameInstance::ResetScreenshots()
{
	CapturedScreenshots.Empty();
	ResetPhotoPoints();
	UE_LOG(LogTemp, Log, TEXT("Screenshots and photo points reset for new game session."));
}

TArray<UTexture2D*> UMyGameInstance::GetAllScreenshots() const
{
	return CapturedScreenshots;
}

void UMyGameInstance::DisplayScreenshotsInGrid(FVector StartLocation, FVector GridSpacing)
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("World is null!"));
		return;
	}

	if (!ScreenshotDisplayActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ScreenshotDisplayActorClass is not set!"));
		return;
	}

	if (CapturedScreenshots.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No screenshots to display!"));
		return;
	}

	int32 Columns = 3;
	int32 Rows = 2;

	for (int32 i = 0; i < CapturedScreenshots.Num(); i++)
	{
		int32 Row = i / Columns;
		int32 Col = i % Columns;

		FVector Location = StartLocation;
		Location.X += 0.0f;
		Location.Y += Col * GridSpacing.Y;
		Location.Z += Row * GridSpacing.Z;

		UE_LOG(LogTemp, Log, TEXT("Screenshot %d: Row=%d, Col=%d, Location=(X=%.2f, Y=%.2f, Z=%.2f), Offset=(Y=%.2f, Z=%.2f)"), 
			i + 1, Row, Col, Location.X, Location.Y, Location.Z, 
			Col * GridSpacing.Y, Row * GridSpacing.Z);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AScreenshotDisplayActor* DisplayActor = GetWorld()->SpawnActor<AScreenshotDisplayActor>(
			ScreenshotDisplayActorClass,
			Location,
			FRotator(0.0f, 270.0f, 90.0f),
			SpawnParams
		);

		if (DisplayActor && CapturedScreenshots[i])
		{
			FVector InitialScale = DisplayActor->GetActorScale3D();
			UE_LOG(LogTemp, Log, TEXT("Screenshot %d: Initial Scale=(%.2f, %.2f, %.2f)"), 
				i + 1, InitialScale.X, InitialScale.Y, InitialScale.Z);
			
			DisplayActor->SetScreenshot(CapturedScreenshots[i]);
			
			FVector NewScale = FVector(3.84f, 2.16f, 1.0f);
			DisplayActor->SetActorScale3D(NewScale);
			
			FVector FinalScale = DisplayActor->GetActorScale3D();
			UE_LOG(LogTemp, Log, TEXT("Screenshot %d: Final Scale=(%.2f, %.2f, %.2f) - Scale change: %s"), 
				i + 1, FinalScale.X, FinalScale.Y, FinalScale.Z,
				FinalScale.Equals(NewScale, 0.01f) ? TEXT("SUCCESS") : TEXT("FAILED"));
			
			UE_LOG(LogTemp, Log, TEXT("Screenshot %d displayed at grid position [Row=%d, Col=%d] successfully"), i + 1, Row, Col);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("All %d screenshots displayed in grid format!"), CapturedScreenshots.Num());
}

UTexture2D* UMyGameInstance::GetLastScreenshot() const
{
	if (CapturedScreenshots.Num() > 0)
	{
		return CapturedScreenshots.Last();
	}
	return nullptr;
}

int32 UMyGameInstance::GetScreenshotCount() const
{
	return CapturedScreenshots.Num();
}

UTexture2D* UMyGameInstance::GetScreenshotAtIndex(int32 Index) const
{
	if (CapturedScreenshots.IsValidIndex(Index))
	{
		return CapturedScreenshots[Index];
	}
	return nullptr;
}
#pragma endregion
