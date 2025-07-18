#include "narisawaBranch/ProceduralRoadGenerator.h"
#include "Components/SplineComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

AProceduralRoadGenerator::AProceduralRoadGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProceduralRoadGenerator::GenerateBuildingsAlongPath(USplineComponent* Path)
{
	if (!Path || BuildingCandidates.Num() == 0)
	{
		return;
	}

	const float PathLength = Path->GetSplineLength();

	for (float Distance = 0.0f; Distance < PathLength; Distance += SpawnInterval)
	{
		const FVector LocationOnSpline = Path->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		const FVector ForwardVector = Path->GetDirectionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		const FVector RightVector = FVector::CrossProduct(ForwardVector, FVector::UpVector).GetSafeNormal();

		auto TraceAndAdjustHeight = [&](FVector& Location)
			{
				FHitResult HitResult;
				FVector TraceStart = Location + FVector(0, 0, 1000.0f);
				FVector TraceEnd = Location - FVector(0, 0, 2000.0f);
				if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility))
				{
					Location.Z = HitResult.ImpactPoint.Z + ZOffset;
				}
			};

		const FRotator BackBuildingRotation = ForwardVector.Rotation();
		const FRotator FrontBuildingRotation = BackBuildingRotation + FRotator(0.0f, 180.0f, 0.0f);

		// --- ?�y�ŏd�v�C���_�z�ʒu�v�Z�̃��W�b�N�𕪗��E���m�� ---

		// �X�e�b�vA: �O������̊�_������
		const FVector FrontBase = LocationOnSpline + (ForwardVector * DistanceOffset);
		const FVector BackBase = LocationOnSpline - (ForwardVector * DistanceOffset);

		// �X�e�b�vB: �������̃V�t�g�ʂ�����
		const FVector LateralShift = RightVector * LateralOffset;

		// �X�e�b�vC: �O��Ɖ��̃I�t�Z�b�g���������āA�N���X�^�[�̍ŏI�I�Ȋ�_���v�Z
		const FVector FrontClusterBaseLocation = FrontBase + LateralShift;
		const FVector BackClusterBaseLocation = BackBase + LateralShift;


		for (int32 i = 0; i < NumBuildingsInCluster; ++i)
		{
			const FVector ClusterOffset = RightVector * (i * BuildingSpacing);

			FVector FrontSpawnLocation = FrontClusterBaseLocation + ClusterOffset;
			TraceAndAdjustHeight(FrontSpawnLocation);
			const int32 RandomIndexFront = FMath::RandRange(0, BuildingCandidates.Num() - 1);
			SpawnBuilding(BuildingCandidates[RandomIndexFront], FTransform(FrontBuildingRotation, FrontSpawnLocation));

			FVector BackSpawnLocation = BackClusterBaseLocation + ClusterOffset;
			TraceAndAdjustHeight(BackSpawnLocation);
			const int32 RandomIndexBack = FMath::RandRange(0, BuildingCandidates.Num() - 1);
			SpawnBuilding(BuildingCandidates[RandomIndexBack], FTransform(BackBuildingRotation, BackSpawnLocation));
		}
	}
}



void AProceduralRoadGenerator::SpawnBuilding(TSoftClassPtr<AActor> BuildingClass, const FTransform& SpawnTransform)
{
	if (BuildingClass.IsPending())
	{
		FStreamableManager& Streamable = UAssetManager::Get().GetStreamableManager();
		Streamable.RequestAsyncLoad(BuildingClass.ToSoftObjectPath(), [this, BuildingClass, SpawnTransform]()
			{
				if (UClass* LoadedClass = BuildingClass.Get())
				{
					GetWorld()->SpawnActor<AActor>(LoadedClass, SpawnTransform);
				}
			});
	}
	else
	{
		if (UClass* LoadedClass = BuildingClass.Get())
		{
			GetWorld()->SpawnActor<AActor>(LoadedClass, SpawnTransform);
		}
	}
}