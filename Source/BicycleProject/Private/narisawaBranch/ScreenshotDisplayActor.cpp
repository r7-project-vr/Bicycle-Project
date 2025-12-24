// Fill out your copyright notice in the Description page of Project Settings.

#include "NarisawaBranch/ScreenshotDisplayActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

AScreenshotDisplayActor::AScreenshotDisplayActor()
	: DisplayWidth(192.0f)
	, DisplayHeight(108.0f)
	, DynamicMaterial(nullptr)
	, BaseMaterial(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	// メッシュコンポーネントの作成
	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	RootComponent = DisplayMesh;

	// 平面メッシュを設定
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
	if (PlaneMesh.Succeeded())
	{
		DisplayMesh->SetStaticMesh(PlaneMesh.Object);
	}

	// コリジョンを無効化
	DisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisplayMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	// デフォルトのスケール設定
	FVector DesiredScale = FVector(DisplayWidth / 100.0f, DisplayHeight / 100.0f, 1.0f);
	SetActorScale3D(DesiredScale);
	
	// デバッグログ追加
	UE_LOG(LogTemp, Log, TEXT("ScreenshotDisplayActor Constructor: Width=%.2f, Height=%.2f, Scale=(%.2f, %.2f, %.2f)"), 
		DisplayWidth, DisplayHeight, DesiredScale.X, DesiredScale.Y, DesiredScale.Z);
}

void AScreenshotDisplayActor::BeginPlay()
{
	Super::BeginPlay();
	
	// BeginPlayでもスケールを確認
	FVector CurrentScale = GetActorScale3D();
	UE_LOG(LogTemp, Log, TEXT("ScreenshotDisplayActor BeginPlay: Current Scale=(%.2f, %.2f, %.2f)"), 
		CurrentScale.X, CurrentScale.Y, CurrentScale.Z);
}

void AScreenshotDisplayActor::SetScreenshot(UTexture2D* Screenshot)
{
	if (!Screenshot)
	{
		UE_LOG(LogTemp, Error, TEXT("Screenshot is null!"));
		return;
	}

	if (!BaseMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("BaseMaterial is not set! Please set it in the Blueprint."));
		return;
	}

	// 動的マテリアルインスタンスを作成
	if (!DynamicMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (!DynamicMaterial)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create dynamic material!"));
			return;
		}
	}

	// テクスチャパラメータを設定
	DynamicMaterial->SetTextureParameterValue(FName("ScreenshotTexture"), Screenshot);

	// メッシュにマテリアルを適用
	DisplayMesh->SetMaterial(0, DynamicMaterial);

	// スケールを再度確認
	FVector CurrentScale = GetActorScale3D();
	UE_LOG(LogTemp, Log, TEXT("Screenshot set successfully! Current Scale=(%.2f, %.2f, %.2f)"), 
		CurrentScale.X, CurrentScale.Y, CurrentScale.Z);
}

void AScreenshotDisplayActor::PlaceInFrontOfPlayer(FVector PlayerLocation, FVector PlayerForward, float Distance)
{
	// プレイヤーの前方に配置
	FVector NewLocation = PlayerLocation + (PlayerForward.GetSafeNormal() * Distance);
	
	SetActorLocation(NewLocation);
	
	// プレイヤーの向きに基づいて回転を計算
	FRotator BaseRotation = PlayerForward.Rotation();
	
	// 目標の回転を設定
	FRotator NewRotation;
	NewRotation.Pitch = 0.0f;
	NewRotation.Yaw = BaseRotation.Yaw - 270.0f;
	NewRotation.Roll = -270.0f;
	
	SetActorRotation(NewRotation);
	
	// スケールも確認
	FVector CurrentScale = GetActorScale3D();
	UE_LOG(LogTemp, Log, TEXT("Screenshot placed - Location: %s, Rotation: (Pitch=%.6f, Yaw=%.6f, Roll=%.6f), Scale=(%.2f, %.2f, %.2f)"), 
		*NewLocation.ToString(), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll,
		CurrentScale.X, CurrentScale.Y, CurrentScale.Z);
}