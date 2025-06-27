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
/// �A�N�^�[���X�g�i�����AUI�A�N�^�[�Ȃǉ��ł��o�^�\�j
/// </summary>
UPROPERTY(EditAnywhere, Category = "ProceduralGeneration")
TArray<TSoftClassPtr<AActor>> ActorCandidates;

/// <summary>
/// ���̃A�N�^�[�𐶐�����ʒu�Ɖ�]
/// </summary>
FVector NextSpawnLocation;
FRotator NextSpawnRotation;

private:
TSoftClassPtr<AActor> CurrentSelectedActor;
void OnActorAssetLoaded();
};