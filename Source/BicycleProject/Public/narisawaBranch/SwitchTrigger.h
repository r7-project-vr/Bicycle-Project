#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StreamableManager.h"
#include "UObject/SoftObjectPtr.h"
#include "SwitchTrigger.generated.h"

class ABuildingBase;

UCLASS()
class BICYCLEPROJECT_API ASwitchTrigger : public AActor
{
    GENERATED_BODY()

public:
    ASwitchTrigger();

    // �X�C�b�`�������ꂽ�Ƃ��ɌĂ΂��֐�
    UFUNCTION(BlueprintCallable, Category = "Building")
    void OnSwitchActivated();

protected:
    virtual void BeginPlay() override;

    // �����Ώۂ̌����N���X
    UPROPERTY(EditAnywhere, Category = "Building")
    TSoftClassPtr<ABuildingBase> BuildingToSpawn;

    // �Ō�ɐ�������������ێ�
    UPROPERTY()
    ABuildingBase* LastSpawnedBuilding = nullptr;

private:
    // ���[�h����
    void SpawnBuildingAsync();
    void SpawnAfterLoad();
};
