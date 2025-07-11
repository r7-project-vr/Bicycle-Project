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
        return; // スプラインが無い、または建物候補がなければ何もしない
    }

    const float PathLength = Path->GetSplineLength();

    // 指定した間隔でスプラインに沿ってループ
    for (float Distance = 0.0f; Distance < PathLength; Distance += SpawnInterval)
    {
        // 現在の距離におけるスプライン上の位置、回転、右方向ベクトルを取得
        const FVector Location = Path->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
        const FRotator Rotation = Path->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
        const FVector RightVector = Path->GetRightVectorAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

        // --- 左右の建物の位置と回転を計算 ---
        const FVector LeftSpawnLocation = Location - (RightVector * SideOffset);
        const FVector RightSpawnLocation = Location + (RightVector * SideOffset);

        // 道に沿うような回転を計算（右側は逆向きにする）
        const FRotator LeftSpawnRotation = Rotation;
        const FRotator RightSpawnRotation = Rotation + FRotator(0.0f, 180.0f, 0.0f);

        // --- 左右に建物をスポーン ---
        // 候補からランダムな建物を選択
        const int32 RandomIndex = FMath::RandRange(0, BuildingCandidates.Num() - 1);
        TSoftClassPtr<AActor> SelectedBuilding = BuildingCandidates[RandomIndex];

        // 非同期でスポーン
        SpawnBuilding(SelectedBuilding, FTransform(LeftSpawnRotation, LeftSpawnLocation));
        SpawnBuilding(SelectedBuilding, FTransform(RightSpawnRotation, RightSpawnLocation));
    }
}

void AProceduralRoadGenerator::SpawnBuilding(TSoftClassPtr<AActor> BuildingClass, const FTransform& SpawnTransform)
{
    if (BuildingClass.IsPending())
    {
        // 非同期ロードが必要な場合
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
        // ロード済みの場合
        if (UClass* LoadedClass = BuildingClass.Get())
        {
            GetWorld()->SpawnActor<AActor>(LoadedClass, SpawnTransform);
        }
    }
}