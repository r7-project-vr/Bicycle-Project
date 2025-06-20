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

    // スイッチが押されたときに呼ばれる関数
    UFUNCTION(BlueprintCallable, Category = "Building")
    void OnSwitchActivated();

protected:
    virtual void BeginPlay() override;

    // 生成対象の建物クラス
    UPROPERTY(EditAnywhere, Category = "Building")
    TSoftClassPtr<ABuildingBase> BuildingToSpawn;

    // 最後に生成した建物を保持
    UPROPERTY()
    ABuildingBase* LastSpawnedBuilding = nullptr;

private:
    // ロード処理
    void SpawnBuildingAsync();
    void SpawnAfterLoad();
};
