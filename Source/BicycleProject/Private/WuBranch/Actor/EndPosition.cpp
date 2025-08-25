// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/EndPosition.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "WuBranch/QuestionGameMode.h"
#include "NiagaraComponent.h"

// Sets default values
AEndPosition::AEndPosition()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = _mesh;

	_finishLineCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("EndLineCollision"));
	_finishLineCollision->SetupAttachment(RootComponent);

	CreateFirework();
}

// Called when the game starts or when spawned
void AEndPosition::BeginPlay()
{
	Super::BeginPlay();
	
	_gameMode = Cast<AQuestionGameMode>(UGameplayStatics::GetGameMode(this));
	OnActorBeginOverlap.AddDynamic(this, &AEndPosition::OnOverlapBeginFinishLine);
}

// Called every frame
void AEndPosition::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEndPosition::OnOverlapBeginFinishLine(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		// 再びトリガーしないように
		_finishLineCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

		//エフェクト
		Fire();

		if (_gameMode)
		{
			_gameMode->PassTheGoal(OtherActor);
		}
	}
}

void AEndPosition::CreateFirework()
{
	_fireworkEffect1 = CreateDefaultSubobject<UNiagaraComponent>(FName("Firework Effect1"));
	_fireworkEffect1->SetupAttachment(RootComponent);
	_fireworkEffect1->bAutoActivate = false;

	_fireworkEffect2 = CreateDefaultSubobject<UNiagaraComponent>(FName("Firework Effect2"));
	_fireworkEffect2->SetupAttachment(RootComponent);
	_fireworkEffect2->bAutoActivate = false;

	_fireworkEffect3 = CreateDefaultSubobject<UNiagaraComponent>(FName("Firework Effect3"));
	_fireworkEffect3->SetupAttachment(RootComponent);
	_fireworkEffect3->bAutoActivate = false;

	_fireworkEffect4 = CreateDefaultSubobject<UNiagaraComponent>(FName("Firework Effect4"));
	_fireworkEffect4->SetupAttachment(RootComponent);
	_fireworkEffect4->bAutoActivate = false;

	_fireworkEffect5 = CreateDefaultSubobject<UNiagaraComponent>(FName("Firework Effect5"));
	_fireworkEffect5->SetupAttachment(RootComponent);
	_fireworkEffect5->bAutoActivate = false;

	_fireworkEffect6 = CreateDefaultSubobject<UNiagaraComponent>(FName("Firework Effect6"));
	_fireworkEffect6->SetupAttachment(RootComponent);
	_fireworkEffect6->bAutoActivate = false;
}

void AEndPosition::Fire()
{
	if (!_fireworkEffect1->IsActive())
	{
		_fireworkEffect1->Activate(true);
	}

	if (!_fireworkEffect2->IsActive())
	{
		_fireworkEffect2->Activate(true);
	}

	if (!_fireworkEffect3->IsActive())
	{
		_fireworkEffect3->Activate(true);
	}

	if (!_fireworkEffect4->IsActive())
	{
		_fireworkEffect4->Activate(true);
	}

	if (!_fireworkEffect5->IsActive())
	{
		_fireworkEffect5->Activate(true);
	}

	if (!_fireworkEffect6->IsActive())
	{
		_fireworkEffect6->Activate(true);
	}
}

