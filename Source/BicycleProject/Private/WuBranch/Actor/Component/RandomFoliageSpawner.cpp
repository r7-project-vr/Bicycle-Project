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

    for (const FSpawnTreeZone& Zone : SpawnZones)
    {
        for (int32 Index = 0; Index < Zone.Amount; ++Index)
        {
			// 範囲内のランダムな位置をゲット
            FVector RandomPointInBox = RandomStream.RandPointInBox(Zone.Zone);
			
			AddFoliageInstance(RandomPointInBox, Zone.Zone);
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
		
		// 生成する予定置の高さがあってない
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
	// Z軸を排除
	ClusterBase.Z = 0;
	FVector RandomUnitVector;
	for (int Step = 0; Step < MaxStep; Step++)
	{
		// 決められた位置でさらにランダムな方向に移動
		RandomUnitVector = GetRandomUnitVector();
		ClusterBase += RandomUnitVector * FoliageType->AverageSpreadDistance;

		int InstancesPerStep = 0;

		for (int SeedIndex = 0; SeedIndex < MaxSeed; SeedIndex++)
		{
			// 木の密度分散するために、木ごとにランダムな位置を決める
			RandomUnitVector = GetRandomUnitVector();
			FVector InstanceLocation = ClusterBase + RandomUnitVector * FoliageType->SpreadVariance;
			
			// 位置の最終確認
			if (!IsInZone2D(InstanceLocation, Zone))
				continue;

			// アクターの所在地が変わるのでインスタンスの位置をワールド座標に変換
			FVector WorldLocation = GetOwner()->GetActorTransform().TransformPosition(InstanceLocation);

			// 木のスケール倍率
			float ScaleFactor = RandomStream.FRandRange(FoliageType->ProceduralScale.Min, FoliageType->ProceduralScale.Max);
			// 木の半径
			float Radius = FoliageType->CollisionRadius;

			FHitResult HitResult;

			FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius * ScaleFactor);
			bool Hit = GetWorld()->SweepSingleByChannel(HitResult, WorldLocation + FVector(0, 0, 10000), WorldLocation + FVector(0, 0, -2000), FQuat::Identity, ECC_Visibility, Sphere);
			
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
			
			// 高さの調整
			FVector OnGroundLocation = HitResult.Location;
			OnGroundLocation.Z = 0;
			FVector AdjustedLocation = OnGroundLocation + FVector(0, 0, RandomStream.FRandRange(FoliageType->ZOffset.Min, FoliageType->ZOffset.Max));

			// トランスフォームを用意
			FTransform InstanceTransform = FTransform();
			InstanceTransform.SetLocation(AdjustedLocation);
			InstanceTransform.SetScale3D(FVector::One() * ScaleFactor);

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

FVector URandomFoliageSpawner::GetRandomUnitVector()
{
	FVector RandomUnitVector = RandomStream.GetUnitVector();
	// Z軸の影響を排除
	RandomUnitVector.Z = 0; 
	RandomUnitVector.Normalize();
	return RandomUnitVector;
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

