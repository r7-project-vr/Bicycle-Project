// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Tablet.h"
#include "Components/BoxComponent.h"
#include "WuBranch/Bike/BikeCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATablet::ATablet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TabletCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	RootComponent = TabletCollision;
	TabletCollision->SetGenerateOverlapEvents(true);
	TabletCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	TabletCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);

	TabletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tablet"));
	TabletMesh->SetupAttachment(RootComponent);
	TabletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Content = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Content"));
	Content->SetupAttachment(TabletMesh);

	Light = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashLight"));
	Light->SetupAttachment(RootComponent);
	Light->SetRelativeLocation(FVector((-1.f, 12.2f, 0.3f)));
	Light->SetRelativeScale3D(FVector(0.001f, 0.001f, 0.001f));
}

// Called when the game starts or when spawned
void ATablet::BeginPlay()
{
	Super::BeginPlay();
	
	if (TabletCollision)
		TabletCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentOverlapBegin);

	if(Light)
		FlashDynamicMaterial = Light->CreateDynamicMaterialInstance(0, Light->GetMaterial(0));
	TextureIndex = 0;
	Hide();
	CloseScene();
}

// Called every frame
//void ATablet::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void ATablet::ShowTutorial(ETutorialType Type)
{
	switch(Type)
	{
	case ETutorialType::Photo:
		CurrentTutorial = PhotoTutorials;
		break;
	case ETutorialType::Coin:
		CurrentTutorial = CoinTutorials;
		break;
	case ETutorialType::SpeedOver:
		CurrentTutorial = SpeedOverTutorials;
		break;
	case ETutorialType::Quiz:
		CurrentTutorial = QuizTutorials;
		break;
	default:
		break;
	}

	if (!CurrentTutorial.IsEmpty())
	{
		TextureIndex = 0;
		ChangeTexture(CurrentTutorial[TextureIndex]);
		Show();
	}
	else
	{
		Hide();
	}
}

void ATablet::SetReferencePoint(FVector Point)
{
	ReferencePoint = Point;
}

void ATablet::SetHideHeight(float Height)
{
	// タブレット本体
	UMaterialInterface* Material = TabletMesh->GetOverlayMaterial();
	UMaterialInstanceDynamic* DynamicMaterial = TabletMesh->CreateDynamicMaterialInstance(0, Material);
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(FName("Height"), Height);
		TabletMesh->SetOverlayMaterial(DynamicMaterial);
	}

	// コンテンツ
	Material = Content->GetOverlayMaterial();
	DynamicMaterial = Content->CreateDynamicMaterialInstance(0, Material);
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(FName("Height"), Height);
		Content->SetOverlayMaterial(DynamicMaterial);
	}
}

void ATablet::CloseScene()
{
	if (!Content)
		return;

	UMaterialInstanceDynamic* Material = Content->CreateDynamicMaterialInstance(0);
	if (Material)
	{
		Material->SetScalarParameterValue(FName("Open"), 0.f);
		Content->SetMaterial(0, Material);
	}
}

void ATablet::OpenScene()
{
	if (!Content)
		return;

	UMaterialInstanceDynamic* Material = Content->CreateDynamicMaterialInstance(0);
	if (Material)
	{
		Material->SetScalarParameterValue(FName("Open"), 1.f);
		Content->SetMaterial(0, Material);
	}
}

void ATablet::EnableCollision()
{
	TabletCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ATablet::DisableCollision()
{
	TabletCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATablet::FlashForNotify()
{
	// 5秒ごとに
	GetWorld()->GetTimerManager().SetTimer(NotifyTimer, [this]() {
		FlashOneTime(0.5f);
		}, 5.f, true, 0.f);
}

void ATablet::RotateScene(bool bIsLeftHand)
{
	FRotator CurrentRotator = Content->GetRelativeRotation();
	if (bIsLeftHand)
		CurrentRotator = FRotator(0.f, -90.f, 0.f);
	else
		CurrentRotator = FRotator(0.f, 90.f, 0.f);
	Content->SetRelativeRotation(CurrentRotator);
}

void ATablet::NextTutorial()
{
	int Size = CurrentTutorial.Num();
	if (TextureIndex < Size - 1)
	{
		TextureIndex++;
		ChangeTexture(CurrentTutorial[TextureIndex]);
	}
}

void ATablet::PreTutorial()
{
	if (TextureIndex > 0)
	{
		TextureIndex--;
		ChangeTexture(CurrentTutorial[TextureIndex]);
	}
}

void ATablet::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ABikeCharacter* Character = Cast<ABikeCharacter>(OtherActor);
		if (Character)
		{
			if (Character->TakeTablet(OtherComp))
			{
				// 通知やフラッシュタイマーを停止
				GetWorld()->GetTimerManager().ClearTimer(NotifyTimer);
				GetWorld()->GetTimerManager().ClearTimer(FlashTimer);
				// 光を消す
				ChangeLightIntensity(0.f);
				OpenScene();
			}
		}
	}
}

void ATablet::ChangeTexture(UTexture2D* Texture)
{
	if (!Texture || !Content)
		return;

	UMaterialInstanceDynamic* Material = Content->CreateDynamicMaterialInstance(0);
	if (Material)
	{
		Material->SetTextureParameterValue(FName("TutorialTexture"), Texture);
		Content->SetMaterial(0, Material);
	}
}

void ATablet::Hide()
{
	SetActorHiddenInGame(true);
	DisableCollision();
}

void ATablet::Show()
{
	SetActorHiddenInGame(false);
}

void ATablet::FlashOneTime(float Sec)
{
	ChangeLightIntensity(10.f);
	if (NotifySE)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), NotifySE, GetActorLocation());

	GetWorld()->GetTimerManager().SetTimer(FlashTimer, [this]() {
		ChangeLightIntensity(0.f);
		}, Sec, false);
}

void ATablet::ChangeLightIntensity(float Intensity)
{
	if (FlashDynamicMaterial)
	{
		FlashDynamicMaterial->SetScalarParameterValue(FName("Intensity"), Intensity);
		if (Intensity > 0.f)
			FlashDynamicMaterial->SetScalarParameterValue(FName("Open"), 1.0f);
		else
			FlashDynamicMaterial->SetScalarParameterValue(FName("Open"), 0.0f);
		Light->SetOverlayMaterial(FlashDynamicMaterial);
	}
}
