#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BuildingSnapInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBuildingSnapInterface : public UInterface
{
GENERATED_BODY()
};

class IBuildingSnapInterface
{
GENERATED_BODY()

public:
UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Snap")
FVector GetSnapLocation() const;

UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Snap")
FRotator GetSnapRotation() const;
};