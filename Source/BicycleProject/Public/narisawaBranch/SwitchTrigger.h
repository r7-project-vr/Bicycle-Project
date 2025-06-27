#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/SoftObjectPtr.h"
#include "narisawaBranch/BuildingSnapInterface.h"
#include "SwitchTrigger.generated.h"

UCLASS()
class BICYCLEPROJECT_API ASwitchTrigger : public AActor, public IBuildingSnapInterface
{
GENERATED_BODY()

public:
ASwitchTrigger();

UFUNCTION(BlueprintCallable, Category = "ProceduralGeneration")
void GenerateNextActor();

protected:
virtual void BeginPlay() override;

/// <summary>
/// アクターリスト（建物、UIアクターなど何でも登録可能）
/// </summary>
UPROPERTY(EditAnywhere, Category = "ProceduralGeneration")
TArray<TSoftClassPtr<AActor>> ActorCandidates;

/// <summary>
/// 次のアクターを生成する位置と回転
/// </summary>
FVector NextSpawnLocation;
FRotator NextSpawnRotation;

private:
TSoftClassPtr<AActor> CurrentSelectedActor;
void OnActorAssetLoaded();
};