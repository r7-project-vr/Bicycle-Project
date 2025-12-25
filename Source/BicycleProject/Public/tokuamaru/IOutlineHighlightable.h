// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IOutlineHighlightable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIOutlineHighlightable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BICYCLEPROJECT_API IIOutlineHighlightable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	void EnableHighlight();

	UFUNCTION(BlueprintNativeEvent)
	void DisableHighlight();



};
