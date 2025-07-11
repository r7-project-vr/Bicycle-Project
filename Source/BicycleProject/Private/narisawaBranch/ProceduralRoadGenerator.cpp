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
        return; // �X�v���C���������A�܂��͌�����₪�Ȃ���Ή������Ȃ�
    }

    const float PathLength = Path->GetSplineLength();

    // �w�肵���Ԋu�ŃX�v���C���ɉ����ă��[�v
    for (float Distance = 0.0f; Distance < PathLength; Distance += SpawnInterval)
    {
        // ���݂̋����ɂ�����X�v���C����̈ʒu�A��]�A�E�����x�N�g�����擾
        const FVector Location = Path->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
        const FRotator Rotation = Path->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
        const FVector RightVector = Path->GetRightVectorAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

        // --- ���E�̌����̈ʒu�Ɖ�]���v�Z ---
        const FVector LeftSpawnLocation = Location - (RightVector * SideOffset);
        const FVector RightSpawnLocation = Location + (RightVector * SideOffset);

        // ���ɉ����悤�ȉ�]���v�Z�i�E���͋t�����ɂ���j
        const FRotator LeftSpawnRotation = Rotation;
        const FRotator RightSpawnRotation = Rotation + FRotator(0.0f, 180.0f, 0.0f);

        // --- ���E�Ɍ������X�|�[�� ---
        // ��₩�烉���_���Ȍ�����I��
        const int32 RandomIndex = FMath::RandRange(0, BuildingCandidates.Num() - 1);
        TSoftClassPtr<AActor> SelectedBuilding = BuildingCandidates[RandomIndex];

        // �񓯊��ŃX�|�[��
        SpawnBuilding(SelectedBuilding, FTransform(LeftSpawnRotation, LeftSpawnLocation));
        SpawnBuilding(SelectedBuilding, FTransform(RightSpawnRotation, RightSpawnLocation));
    }
}

void AProceduralRoadGenerator::SpawnBuilding(TSoftClassPtr<AActor> BuildingClass, const FTransform& SpawnTransform)
{
    if (BuildingClass.IsPending())
    {
        // �񓯊����[�h���K�v�ȏꍇ
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
        // ���[�h�ς݂̏ꍇ
        if (UClass* LoadedClass = BuildingClass.Get())
        {
            GetWorld()->SpawnActor<AActor>(LoadedClass, SpawnTransform);
        }
    }
}