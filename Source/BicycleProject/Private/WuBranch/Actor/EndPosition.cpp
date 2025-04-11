// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/EndPosition.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <WuBranch/Bike/BikeComponent.h>

// Sets default values
AEndPosition::AEndPosition()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_finishLineCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("EndLineCollision"));
	_finishLineCollision->OnComponentBeginOverlap.AddDynamic(this, &AEndPosition::OnOverlapBeginFinishLine);
	AddInstanceComponent(_finishLineCollision);
	RootComponent = _finishLineCollision;

	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	_mesh->SetupAttachment(RootComponent);
	AddInstanceComponent(_mesh);

	_isReadyToChangeLevel = false;
}

// Called when the game starts or when spawned
void AEndPosition::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEndPosition::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEndPosition::OnOverlapBeginFinishLine(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player") && !_isReadyToChangeLevel)
	{
		// 再びトリガーしないように
		_isReadyToChangeLevel = true;

		// 自転車の制御を強制的にオフにする 
		UBikeComponent* bike = OtherActor->GetComponentByClass<UBikeComponent>();
		bike->OpenForcedControl();

		//5秒後にレベルを再読み込み
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AEndPosition::RestartLevel, 5.f, false);
	}
}

void AEndPosition::RestartLevel()
{
	FString map = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), FName(map));
}

