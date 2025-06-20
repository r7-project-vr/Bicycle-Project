#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/StreamableManager.h"
#include "SwitchTrigger.generated.h"

class ABuildingBase;

UCLASS()
class BICYCLEPROJECT_API ASwitchTrigger : public AActor
{
    GENERATED_BODY()

public:
    ASwitchTrigger();

    UFUNCTION(BlueprintCallable, Category = "Building")
    void OnSwitchActivated();

protected:
    virtual void BeginPlay() override;

    // �����̌����N���X�̓o�^
    UPROPERTY(EditAnywhere, Category = "Building")
    TArray<TSoftClassPtr<ABuildingBase>> BuildingCandidates;

    UPROPERTY()
    ABuildingBase* LastSpawnedBuilding = nullptr;

private:
    // ���݂̓ǂݍ��ݑΏ�
    TSoftClassPtr<ABuildingBase> CurrentSelectedBuilding;

    void SpawnBuildingAsync();
    void SpawnAfterLoad();
};
