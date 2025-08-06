// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/EnvironmentalObjectComponent.h"
#include "Engine/StaticMeshActor.h"

// Sets default values for this component's properties
UEnvironmentalObjectComponent::UEnvironmentalObjectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnvironmentalObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CaculateTotalProbility();
	CreateMesh(DecideMesh());
}


// Called every frame
void UEnvironmentalObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEnvironmentalObjectComponent::CaculateTotalProbility()
{
	// 合計確率を初期化
	TotalProbility = 0.0f;

	// Mapに内容がない場合
	if (EnvironmentalList.Num() == 0)
		return;

	// 合計を計算
	TArray<int> Probilities;
	EnvironmentalList.GenerateValueArray(Probilities);
	for (const int& Probility : Probilities)
	{
		TotalProbility += Probility;
	}
}

void UEnvironmentalObjectComponent::CreateMesh(UStaticMesh* Target)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("Create Environmental Object Failed"));
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Orange, FString::Printf(TEXT("Position: %s"), *GetComponentTransform().ToString()));
	EnvironmentalObject = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), GetComponentTransform());
	EnvironmentalObject->SetMobility(EComponentMobility::Stationary);
	if (UStaticMeshComponent* MeshComp = EnvironmentalObject->GetStaticMeshComponent())
	{
		MeshComp->SetStaticMesh(Target);
	}
}

UStaticMesh* UEnvironmentalObjectComponent::DecideMesh()
{
	// １から合計確率までランダムで数値をゲット
	int Target = FMath::RandRange(1, TotalProbility);
	// 
	auto ObjectsArray = EnvironmentalList.Array();
	int Sum = 0;
	UStaticMesh* Object = nullptr;
	for (const auto& ObjectType : ObjectsArray)
	{
		Sum += ObjectType.Value;
		if (Sum >= Target)
		{
			Object = ObjectType.Key;
			break;
		}

	}
	return Object;
}

