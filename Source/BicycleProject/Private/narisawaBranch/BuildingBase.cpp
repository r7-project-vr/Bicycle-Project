
#include "narisawaBranch/BuildingBase.h"

ABuildingBase::ABuildingBase()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    SnapPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SnapPoint"));
    SnapPoint->SetupAttachment(Root);
    SnapPoint->SetRelativeLocation(FVector(1000, 0, 0));
}

FVector ABuildingBase::GetSnapLocation() const
{
    return SnapPoint->GetComponentLocation();
}

FRotator ABuildingBase::GetSnapRotation() const
{
    return SnapPoint->GetComponentRotation();
}
