#include "narisawaBranch/SwitchTrigger.h"
#include "narisawaBranch/BuildingBase.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

ASwitchTrigger::ASwitchTrigger()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASwitchTrigger::BeginPlay()
{
    Super::BeginPlay();
}

void ASwitchTrigger::OnSwitchActivated()
{
    SpawnBuildingAsync();
}

void ASwitchTrigger::SpawnBuildingAsync()
{
    if (!BuildingToSpawn.IsValid())
    {
        FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
        Streamable.RequestAsyncLoad(BuildingToSpawn.ToSoftObjectPath(),
            FStreamableDelegate::CreateUObject(this, &ASwitchTrigger::SpawnAfterLoad));
    }
    else
    {
        SpawnAfterLoad();
    }
}

void ASwitchTrigger::SpawnAfterLoad()
{
    UClass* LoadedClass = BuildingToSpawn.Get();
    if (!LoadedClass) return;

    // 生成位置
    FVector SpawnLocation = FVector::ZeroVector;
    FRotator SpawnRotation = FRotator::ZeroRotator;

    if (LastSpawnedBuilding)
    {
        SpawnLocation = LastSpawnedBuilding->GetSnapLocation();
        SpawnRotation = LastSpawnedBuilding->GetSnapRotation();
    }
    else
    {
        // このスイッチの前方に配置
        SpawnLocation = GetActorLocation() + GetActorForwardVector() * 1000.0f;
    }

    // アクター生成
    FActorSpawnParameters SpawnParams;
    ABuildingBase* NewBuilding = GetWorld()->SpawnActor<ABuildingBase>(LoadedClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (NewBuilding)
    {
        LastSpawnedBuilding = NewBuilding;
    }
}
