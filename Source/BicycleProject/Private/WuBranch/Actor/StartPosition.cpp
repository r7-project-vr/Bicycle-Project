// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/StartPosition.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/GameModeBase.h"
#include <Kismet/KismetSystemLibrary.h>

// Sets default values
AStartPosition::AStartPosition()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	_mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_mesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AddInstanceComponent(_mesh);
	RootComponent = _mesh;
}

// Called when the game starts or when spawned
void AStartPosition::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStartPosition::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

