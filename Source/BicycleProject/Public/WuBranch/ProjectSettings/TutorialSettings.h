// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "TutorialSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Tutorial System"))
class BICYCLEPROJECT_API UTutorialSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Tutorial",
		meta = (RequiredAssetDataTags = "RowStructure=/Script/BicycleProject.TutorialData"))
	TSoftObjectPtr<UDataTable> TutorialDataTable;
};
