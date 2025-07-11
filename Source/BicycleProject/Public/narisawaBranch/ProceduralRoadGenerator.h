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

    /** @brief ���H�̒��S����u�O���v�ɂǂꂾ�������� (X���I�t�Z�b�g) */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    float DistanceOffset = 1500.0f;

    /** @brief ���H�̒��S����u�������v�ɂǂꂾ�������� (Y���I�t�Z�b�g) */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    float LateralOffset = 0.0f;


    /** @brief �n�ʂ̍����ɑ΂��錚����Z���I�t�Z�b�g */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    float ZOffset = 0.0f;

    /** @brief ��x�ɐ������錚���̐��i�N���X�^�[���j */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation", meta = (ClampMin = "1"))
    int32 NumBuildingsInCluster = 3;

    /** @brief �N���X�^�[���̌������m�̊Ԋu */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation", meta = (ClampMin = "0.0"))
    float BuildingSpacing = 2500.0f;

private:
    void SpawnBuilding(TSoftClassPtr<AActor> BuildingClass, const FTransform& SpawnTransform);
};