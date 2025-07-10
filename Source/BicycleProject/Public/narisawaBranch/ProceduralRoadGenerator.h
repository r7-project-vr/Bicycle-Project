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
     * @brief �w�肳�ꂽ�X�v���C���ɉ����āA���̗��e�Ɍ����𐶐����܂��B
     * @param Path �X�v���C���R���|�[�l���g�̃p�X
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    void GenerateBuildingsAlongPath(USplineComponent* Path);

protected:
    /** @brief �������錚���̌�⃊�X�g */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    TArray<TSoftClassPtr<AActor>> BuildingCandidates;

    /** @brief ������z�u����Ԋu�iUnreal���j�b�g�j */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation", meta = (ClampMin = "100.0"))
    float SpawnInterval = 2000.0f;

    /** @brief ���̒��S���獶�E�ɂǂꂾ�������Č�����z�u���邩 */
    UPROPERTY(EditAnywhere, Category = "Procedural Generation", meta = (ClampMin = "100.0"))
    float SideOffset = 1500.0f;

private:
    /**
     * @brief �w�肵���g�����X�t�H�[���Ō�����񓯊��ɃX�|�[������������֐�
     */
    void SpawnBuilding(TSoftClassPtr<AActor> BuildingClass, const FTransform& SpawnTransform);
};