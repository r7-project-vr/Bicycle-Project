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

	int32 Columns = 3;  //横3列
	int32 Rows = 2;     //縦2行

	//新しい順（最新が右下）に表示
	//配列のインデックスが大きいほど新しい写真なので、逆順に取得
	for (int32 i = 0; i < CapturedScreenshots.Num(); i++)
	{
		//最新の写真を左上から配置するため、配列の後ろから取得
		int32 ScreenshotIndex = CapturedScreenshots.Num() - 1 - i;

		int32 Row = i / Columns;
		int32 Col = i % Columns;

		// 配置位置の計算
		FVector Location = StartLocation;
		Location.Y -= Col * GridSpacing.Y;
		Location.Z -= Row * GridSpacing.Z;

		UE_LOG(LogTemp, Log, TEXT("Screenshot %d/%d: Grid[Row=%d, Col=%d] at (Y=%.2f, Z=%.2f)"), 
			i + 1, CapturedScreenshots.Num(), Row, Col, Location.Y, Location.Z);

		// スクリーンショット表示アクターの生成
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AScreenshotDisplayActor* DisplayActor = GetWorld()->SpawnActor<AScreenshotDisplayActor>(
			ScreenshotDisplayActorClass,
			Location,
			FRotator(0.0f, 270.0f, 90.0f),
			SpawnParams
		);

		if (DisplayActor && CapturedScreenshots[ScreenshotIndex])
		{
			DisplayActor->SetScreenshot(CapturedScreenshots[ScreenshotIndex]);
			
			FVector NewScale = FVector(3.84f, 2.16f, 1.0f);
			DisplayActor->SetActorScale3D(NewScale);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Total %d screenshots displayed in grid (newest: top-left, oldest: bottom-right)"), 
		CapturedScreenshots.Num());
}