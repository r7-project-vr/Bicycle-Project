// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include <Components/WidgetComponent.h>
#include <tokuamaru/TimerWidget.h>
#include "GameFramework/Actor.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Sound/SoundBase.h"
#include "InputActionValue.h"
#include "TimeManager.generated.h"

UCLASS()
class BICYCLEPROJECT_API ATimeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimeManager(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void EventAction(const FInputActionValue& Value);

	void EventAxis(const FInputActionValue& Value);



	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* ActionInput;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* AxisInput;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float timer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* _widgetT;
	UTimerWidget* TimerW;

	//ディスパッチャー
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimeEndDelegate);
	UPROPERTY(BlueprintAssignable)
	FTimeEndDelegate timeEnd;

	UFUNCTION()
	void printst();

private:
	void SetupInput();

	void PressedAction();
	void ReleasedAction();

	void PressedAxis(const FInputActionValue& Value);

	bool stoper;
	bool soundPlay;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* DefaultMappingContext;

	void TimerStop();

	void SoundPlay();

	UPROPERTY(EditAnywhere,Category = "Sound")
	USoundBase* SoundToPlay;

};
