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

    /**
     * @brief 指定されたスプラインに沿って、道の両脇に建物を生成します。
     * @param Path スプラインコンポーネントのパス
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    void GenerateBuildingsAlongPath(USplineComponent* Path);

protected:
    /** @brief 生成する建物の候補リスト */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    TArray<TSoftClassPtr<AActor>> BuildingCandidates;

    /** @brief 建物を配置する間隔（Unrealユニット） */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation", meta = (ClampMin = "100.0"))
    float SpawnInterval = 2000.0f;

    /** @brief 道の中心から左右にどれだけ離して建物を配置するか */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation", meta = (ClampMin = "100.0"))
    float SideOffset = 1500.0f;

private:
    /**
     * @brief 指定したトランスフォームで建物を非同期にスポーンさせる内部関数
     */
    void SpawnBuilding(TSoftClassPtr<AActor> BuildingClass, const FTransform& SpawnTransform);
};