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
    if (BuildingCandidates.Num() == 0) return;

    // ƒ‰ƒ“ƒ_ƒ€‚É1‚Â‘I‘ð
    int32 Index = FMath::RandRange(0, BuildingCandidates.Num() - 1);
    CurrentSelectedBuilding = BuildingCandidates[Index];

    if (!CurrentSelectedBuilding.IsValid())
    {
        FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
        Streamable.RequestAsyncLoad(CurrentSelectedBuilding.ToSoftObjectPath(),
            FStreamableDelegate::CreateUObject(this, &ASwitchTrigger::SpawnAfterLoad));
    }
    else
    {
        SpawnAfterLoad();
    }
}

void ASwitchTrigger::SpawnAfterLoad()
{
    UClass* LoadedClass = CurrentSelectedBuilding.Get();
    if (!LoadedClass) return;

    FVector SpawnLocation = FVector::ZeroVector;
    FRotator SpawnRotation = FRotator::ZeroRotator;

    if (LastSpawnedBuilding)
    {
        SpawnLocation = LastSpawnedBuilding->GetSnapLocation();
        SpawnRotation = LastSpawnedBuilding->GetSnapRotation();
    }
    else
    {
        SpawnLocation = GetActorLocation() + GetActorForwardVector() * 1000.0f;
    }

    FActorSpawnParameters SpawnParams;
    ABuildingBase* NewBuilding = GetWorld()->SpawnActor<ABuildingBase>(LoadedClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (NewBuilding)
    {
        LastSpawnedBuilding = NewBuilding;
    }
}
