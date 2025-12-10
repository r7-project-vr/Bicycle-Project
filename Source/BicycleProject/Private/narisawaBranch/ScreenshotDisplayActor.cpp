// Fill out your copyright notice in the Description page of Project Settings.

#include "NarisawaBranch/ScreenshotDisplayActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

AScreenshotDisplayActor::AScreenshotDisplayActor()
	: DisplayWidth(200.0f)
	, DisplayHeight(150.0f)
{
	PrimaryActorTick.bCanEverTick = false;

	// メッシュコンポーネントの作成
	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	RootComponent = DisplayMesh;

	// 平面メッシュを設定（Engine標準のPlane）
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
	if (PlaneMesh.Succeeded())
	{
		DisplayMesh->SetStaticMesh(PlaneMesh.Object);
	}

	// コリジョンを無効化
	DisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisplayMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	// デフォルトのスケール設定
	SetActorScale3D(FVector(DisplayWidth / 100.0f, DisplayHeight / 100.0f, 1.0f));
}

void AScreenshotDisplayActor::BeginPlay()
{
	Super::BeginPlay();
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

	UE_LOG(LogTemp, Log, TEXT("Screenshot set successfully!"));
}

void AScreenshotDisplayActor::PlaceInFrontOfPlayer(FVector PlayerLocation, FVector PlayerForward, float Distance)
{
	// プレイヤーの前方に配置（高さは変えない）
	FVector NewLocation = PlayerLocation + (PlayerForward.GetSafeNormal() * Distance);
	
	SetActorLocation(NewLocation);
	
	// プレイヤーの向きに基づいて回転を計算
	// PlayerForwardからYaw（水平回転）を取得
	FRotator BaseRotation = PlayerForward.Rotation();
	
	// 目標の回転を設定: Pitch=0, Yaw=PlayerのYaw-270, Roll=-270
	FRotator NewRotation;
	NewRotation.Pitch = 0.0f;
	NewRotation.Yaw = BaseRotation.Yaw - 270.0f;  // プレイヤーの向きから-270度
	NewRotation.Roll = -270.0f;
	
	SetActorRotation(NewRotation);
	
	UE_LOG(LogTemp, Log, TEXT("Screenshot placed - Location: %s, Rotation: (Pitch=%.6f, Yaw=%.6f, Roll=%.6f)"), 
		*NewLocation.ToString(), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll);
}