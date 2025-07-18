#include "narisawaBranch/SwitchTrigger.h"
#include "WuBranch/Bike/BikeComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

ASwitchTrigger::ASwitchTrigger()
{
    PrimaryActorTick.bCanEverTick = false;
    NextSpawnLocation = FVector::ZeroVector;
    NextSpawnRotation = FRotator::ZeroRotator;
}

void ASwitchTrigger::BeginPlay()
{
    Super::BeginPlay();

    //����̐����ʒu���X�C�b�`�̑O�ɐݒ�
    if (NextSpawnLocation.IsZero())
    {
        NextSpawnLocation = GetActorLocation() + GetActorForwardVector() * 1000.0f;
        //�����X�|�[���̉�]���X�C�b�`�̉�]�ɍ��킹��
        NextSpawnRotation = GetActorRotation();
    }
}

void ASwitchTrigger::GenerateNextActor()
{
    if (ActorCandidates.Num() == 0)
    {
        return;
    }

    //��₩�烉���_���Ɉ�I��
    const int32 Index = FMath::RandRange(0, ActorCandidates.Num() - 1);
    CurrentSelectedActor = ActorCandidates[Index];

    //�񓯊����[�h
    if (CurrentSelectedActor.IsPending())
    {
        FStreamableManager& Streamable = UAssetManager::Get().GetStreamableManager();
        Streamable.RequestAsyncLoad(CurrentSelectedActor.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ASwitchTrigger::OnActorAssetLoaded));
    }
    else
    {
        OnActorAssetLoaded();
    }
}

void ASwitchTrigger::OnActorAssetLoaded()
{
    UClass* LoadedClass = CurrentSelectedActor.Get();
    if (!LoadedClass)
    {
        return;
    }

    //���݂�NextSpawnLocation,Rotation���g���ăA�N�^�[�𐶐�
    FActorSpawnParameters SpawnParams;
    AActor* NewActor = GetWorld()->SpawnActor<AActor>(LoadedClass, NextSpawnLocation, NextSpawnRotation, SpawnParams);

    //���������A�N�^�[��Snap�C���^�[�t�F�[�X���������Ă��邩�m�F
    if (NewActor && NewActor->GetClass()->ImplementsInterface(UBuildingSnapInterface::StaticClass()))
    {
        //�������Ă���΁A�������玟�̐����ʒu���擾���čX�V
        NextSpawnLocation = IBuildingSnapInterface::Execute_GetSnapLocation(NewActor);
        NextSpawnRotation = IBuildingSnapInterface::Execute_GetSnapRotation(NewActor);
    }
    //�����������Ă��Ȃ���΁ANextSpawnLocation�͍X�V���ꂸ�A���̃A�N�^�[�������ꏊ�̋߂��ɐ��������\��������i�d�l�j
}