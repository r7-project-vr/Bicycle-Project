#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "narisawaBranch/BuildingSnapInterface.h"
#include "BuildingBase.generated.h"


UCLASS()
class BICYCLEPROJECT_API ABuildingBase : public AActor, public IBuildingSnapInterface
{
GENERATED_BODY()

public:
ABuildingBase();

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snap")
USceneComponent* Root;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snap")
USceneComponent* SnapPoint;

FVector GetSnapLocation() const;
FRotator GetSnapRotation() const;
};
