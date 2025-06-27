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

    //�A�N�^�[���X�g�i�����AUI�A�N�^�[�Ȃǉ��ł��o�^�\�j
    UPROPERTY(EditAnywhere, Category = "ProceduralGeneration")
    TArray<TSoftClassPtr<AActor>> ActorCandidates;

    //���̃A�N�^�[�𐶐�����ʒu�Ɖ�]
    FVector NextSpawnLocation;
    FRotator NextSpawnRotation;

private:
    TSoftClassPtr<AActor> CurrentSelectedActor;
    void OnActorAssetLoaded();
};