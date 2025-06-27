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

    //初回の生成位置をスイッチの前に設定
    if (NextSpawnLocation.IsZero())
    {
        NextSpawnLocation = GetActorLocation() + GetActorForwardVector() * 1000.0f;
        //初期スポーンの回転をスイッチの回転に合わせる
        NextSpawnRotation = GetActorRotation();
    }
}

void ASwitchTrigger::GenerateNextActor()
{
    if (ActorCandidates.Num() == 0)
    {
        return;
    }

    //候補からランダムに一つ選択
    const int32 Index = FMath::RandRange(0, ActorCandidates.Num() - 1);
    CurrentSelectedActor = ActorCandidates[Index];

    //非同期ロード
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

    //現在のNextSpawnLocation,Rotationを使ってアクターを生成
    FActorSpawnParameters SpawnParams;
    AActor* NewActor = GetWorld()->SpawnActor<AActor>(LoadedClass, NextSpawnLocation, NextSpawnRotation, SpawnParams);

    //生成したアクターがSnapインターフェースを実装しているか確認
    if (NewActor && NewActor->GetClass()->ImplementsInterface(UBuildingSnapInterface::StaticClass()))
    {
        //実装していれば、そこから次の生成位置を取得して更新
        NextSpawnLocation = IBuildingSnapInterface::Execute_GetSnapLocation(NewActor);
        NextSpawnRotation = IBuildingSnapInterface::Execute_GetSnapRotation(NewActor);
    }
    //もし実装していなければ、NextSpawnLocationは更新されず、次のアクターも同じ場所の近くに生成される可能性がある（仕様）
}