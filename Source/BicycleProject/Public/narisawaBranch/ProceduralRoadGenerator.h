#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralRoadGenerator.generated.h"

class USplineComponent;

UCLASS()
class BICYCLEPROJECT_API AProceduralRoadGenerator : public AActor
{
    GENERATED_BODY()

public:
    AProceduralRoadGenerator();

    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    void GenerateBuildingsAlongPath(USplineComponent* Path);

protected:
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    TArray<TSoftClassPtr<AActor>> BuildingCandidates;

    UPROPERTY(EditAnywhere, Category = "Procedural Generation", meta = (ClampMin = "100.0"))
    float SpawnInterval = 2000.0f;

    /** @brief 道路の中心から「前方」にどれだけ離すか (X軸オフセット) */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    float DistanceOffset = 1500.0f;

    /** @brief 道路の中心から「横方向」にどれだけ離すか (Y軸オフセット) */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    float LateralOffset = 0.0f;


    /** @brief 地面の高さに対する建物のZ軸オフセット */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    float ZOffset = 0.0f;

    /** @brief 一度に生成する建物の数（クラスター数） */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation", meta = (ClampMin = "1"))
    int32 NumBuildingsInCluster = 3;

    /** @brief クラスター内の建物同士の間隔 */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation", meta = (ClampMin = "0.0"))
    float BuildingSpacing = 2500.0f;

private:
    void SpawnBuilding(TSoftClassPtr<AActor> BuildingClass, const FTransform& SpawnTransform);
};