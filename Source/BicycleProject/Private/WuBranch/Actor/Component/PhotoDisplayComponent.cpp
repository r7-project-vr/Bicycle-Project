// Fill out your copyright notice in the Description page of Project Settings.

#include "WuBranch/Actor/Component/PhotoDisplayComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/MyGameInstance.h"
#include "NarisawaBranch/ScreenshotDisplayActor.h"
#include "Engine/Texture2D.h"

UPhotoDisplayComponent::UPhotoDisplayComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPhotoDisplayComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPhotoDisplayComponent::DisplayScreenshotsInGrid(FVector StartLocation, FVector GridSpacing)
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

	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is null!"));
		return;
	}

	TArray<UTexture2D*> CapturedScreenshots = GameInstance->GetAllScreenshots();

	if (CapturedScreenshots.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No screenshots to display!"));
		return;
	}

	int32 Columns = 3;
	int32 Rows = 2;

	// 古い順に表示する
	for (int32 i = 0; i < CapturedScreenshots.Num(); i++)
	{
		int32 Row = i / Columns;
		int32 Col = i % Columns;

		FVector Location = StartLocation;
		Location.X += 0.0f;
		Location.Y += Col * GridSpacing.Y;
		Location.Z += Row * GridSpacing.Z;

		UE_LOG(LogTemp, Log, TEXT("Screenshot %d (Order: %d): Row=%d, Col=%d, Location=(X=%.2f, Y=%.2f, Z=%.2f)"), 
			i + 1, i + 1, Row, Col, Location.X, Location.Y, Location.Z);

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
			DisplayActor->SetScreenshot(CapturedScreenshots[i]);
			
			FVector NewScale = FVector(3.84f, 2.16f, 1.0f);
			DisplayActor->SetActorScale3D(NewScale);
			
			UE_LOG(LogTemp, Log, TEXT("Screenshot %d displayed at grid position [Row=%d, Col=%d] - Order: oldest to newest"), 
				i + 1, Row, Col);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("All %d screenshots displayed in chronological order (oldest first)!"), CapturedScreenshots.Num());
}