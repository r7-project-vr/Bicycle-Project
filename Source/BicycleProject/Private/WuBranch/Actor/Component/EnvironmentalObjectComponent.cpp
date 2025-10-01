// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/EnvironmentalObjectComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/AssetManager.h"
#include <Engine/StreamableManager.h>

// Sets default values for this component's properties
UEnvironmentalObjectComponent::UEnvironmentalObjectComponent()
	: EnvironmentalObject(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnvironmentalObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
//void UEnvironmentalObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UEnvironmentalObjectComponent::StartSpawnEnvironmentalObject()
{
	CaculateTotalProbility();
	LoadResource(DecideMesh());
}

void UEnvironmentalObjectComponent::DestroyEnvironmental()
{
	if (EnvironmentalObject)
	{
		EnvironmentalObject->Destroy();
	}
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

void UEnvironmentalObjectComponent::LoadResource(TSoftObjectPtr<UStaticMesh> Target)
{
	if (Target.IsPending())
	{
		FStreamableManager& Streamable = UAssetManager::Get().GetStreamableManager();
		Streamable.RequestAsyncLoad(Target.ToSoftObjectPath(), [this, Target]()
			{
				if (UStaticMesh* Building = Target.Get())
					CreateBuilding(Building);
			});
	}
	else
	{
		if(UStaticMesh* Building = Target.Get())
			CreateBuilding(Building);
	}
}

void UEnvironmentalObjectComponent::CreateBuilding(UStaticMesh* Target)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("Create Environmental Object Failed"));
		return;
	}

	EnvironmentalObject = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), GetComponentTransform());
	EnvironmentalObject->SetMobility(EComponentMobility::Stationary);
	if (UStaticMeshComponent* MeshComp = EnvironmentalObject->GetStaticMeshComponent())
	{
		MeshComp->SetStaticMesh(Target);
	}
}

TSoftObjectPtr<UStaticMesh> UEnvironmentalObjectComponent::DecideMesh()
{
	// １から合計確率までランダムで数値をゲット
	int Target = FMath::RandRange(1, TotalProbility);
	// 
	auto ObjectsArray = EnvironmentalList.Array();
	int Sum = 0;
	TSoftObjectPtr<UStaticMesh> Object = nullptr;
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

