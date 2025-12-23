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

UMyGameInstance::UMyGameInstance()
	: TotalCoins(0)
	, CoinsPerGame(0)
	, CoinHeight(475.f)
	, IsClear(false)
	, StandardRPM(50)
	, RPMThreshold(10)
	, MaxStandardRPM(60)
	// 2025.11.09 谷村 start
	, NumOfSets(4)
	// 2025.11.09 谷村 end
	, MaxAnimalCount(10)
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
	UpdateCoin();
}

void UMyGameInstance::AddCoinsPerGame(int Amount)
{
	// 無効な値
	if(Amount <= 0)
		return;

	CoinsPerGame += Amount;
}

void UMyGameInstance::ResetCoinsPerGame()
{
	CoinsPerGame = 0;
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
	UpdateCoinHeight();
}

void UMyGameInstance::ResetCoinHeight()
{
	CoinHeight = 100.f;
	UpdateCoinHeight();
}

void UMyGameInstance::SaveCoinsToFile(FPlayerSaveGame& Data)
{
	Data.Coins = TotalCoins;
}

void UMyGameInstance::ReadCoinFromFile(const FPlayerSaveGame& Data)
{
	TotalCoins = Data.Coins;
	CoinHeight = 100.f;
	UpdateCoinHeight();
}

void UMyGameInstance::UpdateCoin()
{
	if (OnUpdateCoin.IsBound())
	{
		OnUpdateCoin.Broadcast(TotalCoins);
	}
}

void UMyGameInstance::UpdateCoinHeight()
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

// 2025.11.12 谷村 start
#pragma region セット数
void UMyGameInstance::SetNumOfSets(int Value)
{
	NumOfSets = Value;
}

int32 UMyGameInstance::GetNumOfSets() const
{
	return NumOfSets;
}

// 2025.12.05 ウー start
void UMyGameInstance::SaveSetsToFile(FPlayerSaveGame& Data)
{
	Data.MapSets = NumOfSets;
}

void UMyGameInstance::ReadSetsFromFile(const FPlayerSaveGame& Data)
{
	NumOfSets = Data.MapSets;
}
// 2025.12.05 ウー end
#pragma endregion
// 2025.11.12 谷村 end

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

void UMyGameInstance::SavePhotoToFile(FPlayerSaveGame& Data)
{
	Data.AnimalPhotos = AnimalPhotoNums;
}

void UMyGameInstance::ReadPhotoFromFile(const FPlayerSaveGame& Data)
{
	AnimalPhotoNums = Data.AnimalPhotos;
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
	// データ作り
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

	// カメラの位置と向きを取得
	FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
	FRotator CameraRotation = PC->PlayerCameraManager->GetCameraRotation();

	// Scene Capture 2D アクターを一時的に生成
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	ASceneCapture2D* SceneCapture = GetWorld()->SpawnActor<ASceneCapture2D>(ASceneCapture2D::StaticClass(), CameraLocation, CameraRotation, SpawnParams);
	
	if (!SceneCapture || !SceneCapture->GetCaptureComponent2D())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create SceneCapture2D!"));
		return;
	}

	// レンダーターゲットを作成
	int32 Width = 1920;  // 解像度を指定
	int32 Height = 1080;
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
	RenderTarget->InitAutoFormat(Width, Height);
	RenderTarget->UpdateResourceImmediate(true);

	// Scene Capture の設定
	USceneCaptureComponent2D* CaptureComponent = SceneCapture->GetCaptureComponent2D();
	CaptureComponent->TextureTarget = RenderTarget;
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->bCaptureOnMovement = false;
	
	// キャプチャを実行
	CaptureComponent->CaptureScene();

	// レンダーターゲットからピクセルデータを読み取る
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
		// UTexture2Dを作成
		UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
		if (NewTexture)
		{
#if WITH_EDITORONLY_DATA
			NewTexture->MipGenSettings = TMGS_NoMipmaps;
#endif
			NewTexture->NeverStream = true;
			NewTexture->SRGB = true;

			// テクスチャにピクセルデータをコピー
			void* TextureData = NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, OutBitmap.GetData(), OutBitmap.Num() * sizeof(FColor));
			NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
			NewTexture->UpdateResource();
			// 配列に保存
			CapturedScreenshots.Add(NewTexture);
			
			UE_LOG(LogTemp, Log, TEXT("VR Screenshot captured from camera! Size: %dx%d, Total screenshots: %d"), 
				Width, Height, CapturedScreenshots.Num());
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

	// Scene Capture アクターを削除
	SceneCapture->Destroy();
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

AScreenshotDisplayActor* UMyGameInstance::DisplayLastScreenshot(FVector PlayerLocation, FVector PlayerForward)
{
	UTexture2D* LastScreenshot = GetLastScreenshot();
	if (!LastScreenshot)
	{
		UE_LOG(LogTemp, Warning, TEXT("No screenshot to display!"));
		return nullptr;
	}

	if (!ScreenshotDisplayActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ScreenshotDisplayActorClass is not set!"));
		return nullptr;
	}

	// アクターをスポーン
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AScreenshotDisplayActor* DisplayActor = GetWorld()->SpawnActor<AScreenshotDisplayActor>(
		ScreenshotDisplayActorClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (DisplayActor)
	{
		// スクリーンショットを設定
		DisplayActor->SetScreenshot(LastScreenshot);
		// プレイヤーの前に配置
		DisplayActor->PlaceInFrontOfPlayer(PlayerLocation, PlayerForward);

		UE_LOG(LogTemp, Log, TEXT("Screenshot displayed in 3D space!"));
		return DisplayActor;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to spawn ScreenshotDisplayActor!"));
	return nullptr;
}
#pragma endregion
