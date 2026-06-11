// Fill out your copyright notice in the Description page of Project Settings.

#include "WuBranch/Actor/Component/PhotoDisplayComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/MyGameInstance.h"
#include "NarisawaBranch/ScreenshotDisplayActor.h"
#include "Engine/Texture2D.h"
#include "WuBranch/Struct/AnimalPhoto.h"

UPhotoDisplayComponent::UPhotoDisplayComponent()
	: DelayTime(2.0f)
{
	//PrimaryComponentTick.bCanEverTick = false;
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

	TArray<FAnimalPhoto> CapturedScreenshots = GameInstance->GetAllScreenshots();

	if (CapturedScreenshots.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No screenshots to display!"));
		return;
	}

	//新しい順（最新が右下）に表示
	//配列のインデックスが大きいほど新しい写真なので、逆順に取得
	for (int32 ScreenshotIndex = 0; ScreenshotIndex < CapturedScreenshots.Num(); ScreenshotIndex++)
	{
		int32 Row = ScreenshotIndex / PhotoColumns;
		int32 Col = ScreenshotIndex % PhotoColumns;

		// 配置位置の計算
		FVector Location = StartLocation;
		Location.Y += Col * GridSpacing.Y;
		Location.Z += Row * GridSpacing.Z;

		UE_LOG(LogTemp, Log, TEXT("Screenshot %d/%d: Grid[Row=%d, Col=%d] at (Y=%.2f, Z=%.2f)"), 
			ScreenshotIndex + 1, CapturedScreenshots.Num(), Row, Col, Location.Y, Location.Z);

		// スクリーンショット表示アクターの生成
		FTimerHandle DisplayTimer;
		FTimerDelegate DisplayDelegate;
		DisplayDelegate.BindUFunction(this, FName("MakeDisplayActor"), Location, CapturedScreenshots[ScreenshotIndex]);

		GetOwner()->GetWorldTimerManager().SetTimer(DisplayTimer, DisplayDelegate, ScreenshotIndex * DelayTime + 0.1f, false);
		//MakeDisplayActor(Location, CapturedScreenshots[ScreenshotIndex]);
	}

	UE_LOG(LogTemp, Log, TEXT("Total %d screenshots displayed in grid (newest: top-left, oldest: bottom-right)"), 
		CapturedScreenshots.Num());
}

void UPhotoDisplayComponent::MakeDisplayActor(FVector Location, const FAnimalPhoto& PhotoData)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Location : %s"), *Location.ToString()));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AScreenshotDisplayActor* DisplayActor = GetWorld()->SpawnActor<AScreenshotDisplayActor>(
		ScreenshotDisplayActorClass,
		Location,
		FRotator(0.0f, 270.0f, 90.0f),
		SpawnParams
	);

	if (DisplayActor)
	{
		DisplayActor->SetScreenshot(PhotoData.Photo);
		DisplayActor->SetText(PhotoData.Names);

		FVector NewScale = FVector(3.84f, 2.16f, 1.0f);
		DisplayActor->SetActorScale3D(NewScale);
	}
}
