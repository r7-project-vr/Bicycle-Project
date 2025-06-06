// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/EndPosition.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "WuBranch/QuestionGameMode.h"

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
}

// Called when the game starts or when spawned
void AEndPosition::BeginPlay()
{
	Super::BeginPlay();
	
	_gameMode = Cast<AQuestionGameMode>(UGameplayStatics::GetGameMode(this));
}

// Called every frame
void AEndPosition::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEndPosition::OnOverlapBeginFinishLine(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		// 再びトリガーしないように
		_finishLineCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

		if (_gameMode)
		{
			_gameMode->PassTheGoal(OtherActor);
		}
	}
}

