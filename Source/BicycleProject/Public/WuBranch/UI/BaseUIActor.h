// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseUIActor.generated.h"

class UWidgetComponent;

UCLASS()
class BICYCLEPROJECT_API ABaseUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// UIを表示する
	/// </summary>
	void DisplayUI();

	/// <summary>
	/// UIを表示しない
	/// </summary>
	void NotDisplayUI();

	/// <summary>
	/// UIをゲット
	/// </summary>
	/// <returns>UI</returns>
	UWidgetComponent* GetWidgetComponent();

protected:

	/// <summary>
	/// UIを表示させるもの
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* Widget;
};
