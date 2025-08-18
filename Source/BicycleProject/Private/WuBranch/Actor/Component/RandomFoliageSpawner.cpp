// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/RandomFoliageSpawner.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
URandomFoliageSpawner::URandomFoliageSpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void URandomFoliageSpawner::BeginPlay()
{
	Super::BeginPlay();

	// ...		
}

// Called every frame
//void URandomFoliageSpawner::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void URandomFoliageSpawner::SetSeed(int Seed)
{
	InitialSeed = Seed;
	RandomStream.Initialize(InitialSeed);
}

void URandomFoliageSpawner::StartSpawnFoliage()
{
	InitializeFoliageTypes();

	if (FoliageIsmComponents.Num() > 0)
		SpawnFoliageInZone();
}

void URandomFoliageSpawner::SpawnFoliageInZone()
{
    UWorld* World = GetWorld();
    if (!World || SpawnZones.Num() == 0)
    {
        return;
    }

    for (const FBox& Zone : SpawnZones)
    {
        for (int32 i = 0; i < NumberOfInstancesPerZone; ++i)
        {
			// 範囲内のランダムな位置をゲット
            FVector RandomPointInBox = RandomStream.RandPointInBox(Zone);
			
			AddFoliageInstance(RandomPointInBox, Zone);
        }
    }
}

void URandomFoliageSpawner::RemoveFoliageInstances()
{
	if (FoliageIsmComponents.Num() <= 0)
		return;

	for(UInstancedStaticMeshComponent* FoliageIsmComponent : FoliageIsmComponents)
	{
		if (FoliageIsmComponent)
		{
			FoliageIsmComponent->ClearInstances();
		}
	}
}

void URandomFoliageSpawner::InitializeFoliageTypes()
{
	for (UFoliageType_InstancedStaticMesh* FoliageType : FoliageTypes)
	{
		UInstancedStaticMeshComponent* ISMComponent = Cast<UInstancedStaticMeshComponent>(GetOwner()->AddComponentByClass(UInstancedStaticMeshComponent::StaticClass(), false, FTransform::Identity, false));
		if (ISMComponent)
		{
			// インスタンスの設定
			ISMComponent->SetStaticMesh(FoliageType->GetStaticMesh());
			FoliageIsmComponents.Add(ISMComponent);
		}
	}
}

void URandomFoliageSpawner::AddFoliageInstance(const FVector InLocation, const FBox& Zone)
{
	for (int Index = 0; Index < FoliageTypes.Num(); Index++)
	{
		UFoliageType_InstancedStaticMesh* FoliageType = FoliageTypes[Index];

		// FoliageTypeが有効でない場合はスキップ
		if (!FoliageType)
			continue;
		
		if (InLocation.Z < FoliageType->Height.Min || InLocation.Z > FoliageType->Height.Max)
			continue;

		if (FoliageType->InitialSeedDensity < RandomStream.FRandRange(0.f, 10.f))
			continue;

		UInstancedStaticMeshComponent* FoliageIsmComponent = FoliageIsmComponents.IsValidIndex(Index) ? FoliageIsmComponents[Index] : nullptr;
		if(FoliageIsmComponent)
			SpawnFoliageCluster(FoliageType, FoliageIsmComponent, InLocation, Zone);
	}
}

void URandomFoliageSpawner::SpawnFoliageCluster(UFoliageType_InstancedStaticMesh* FoliageType, UInstancedStaticMeshComponent* FoliageIsmComponent, const FVector& ClusterLocation, const FBox& Zone)
{
	int MaxStep = RandomStream.RandRange(1, FoliageType->NumSteps);
	int MaxSeed = RandomStream.RandRange(1, FoliageType->SeedsPerStep);
	FVector ClusterBase = ClusterLocation;
	for (int Step = 0; Step < MaxStep; Step++)
	{
		ClusterBase += RandomStream.GetUnitVector() * FoliageType->AverageSpreadDistance;

		int InstancesPerStep = 0;

		for (int SeedIndex = 0; SeedIndex < MaxSeed; SeedIndex++)
		{
			FVector InstanceLocation = ClusterBase + RandomStream.GetUnitVector() * FoliageType->SpreadVariance;
			
			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;

			bool Hit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				InstanceLocation + FVector(0, 0, 2000),
				InstanceLocation + FVector(0, 0, -2000),
				ECC_Visibility,
				CollisionParams
			);

			if (!Hit)
				continue;

			// 地面の確認
			if (!HitResult.GetActor()->ActorHasTag("Tile"))
				continue;	

			// 地面の角度を確認
			float DotProduct = FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector);
			float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
			if (SlopeAngle < FoliageType->GroundSlopeAngle.Min || SlopeAngle > FoliageType->GroundSlopeAngle.Max)
				continue;

			// 位置の最終確認
			if (!IsInZone2D(HitResult.Location, Zone))
				continue;
			
			// ヒット位置 + Zオフセット
			FVector AdjustedLocation = HitResult.Location + FVector(0, 0, RandomStream.FRandRange(FoliageType->ZOffset.Min, FoliageType->ZOffset.Max));
			// インスタンスの位置をワールド座標に変換
			FVector WorldLocation = GetOwner()->GetActorTransform().TransformPosition(AdjustedLocation);

			// トランスフォームを用意
			FTransform InstanceTransform = FTransform();
			InstanceTransform.SetLocation(WorldLocation);
			InstanceTransform.SetScale3D(FVector::One() * RandomStream.FRandRange(FoliageType->ProceduralScale.Min, FoliageType->ProceduralScale.Max));

			if (FoliageType->RandomYaw)
			{
				InstanceTransform.SetRotation(FRotator(0, RandomStream.FRandRange(0, 360), 0).Quaternion());
			}

			FoliageIsmComponent->AddInstance(InstanceTransform, true);
			InstancesPerStep++;
		}

		// ステップごとに生成されたインスタンス数が0の場合は
		if (InstancesPerStep == 0)
			return;
	}
}

bool URandomFoliageSpawner::IsInZone2D(const FVector& Location, const FBox& Zone)
{
	// 平面上のチェック
	if(Location.X >= Zone.Min.X && Location.X <= Zone.Max.X &&
	   Location.Y >= Zone.Min.Y && Location.Y <= Zone.Max.Y)
	{
		return true;
	}
	return false;
}

