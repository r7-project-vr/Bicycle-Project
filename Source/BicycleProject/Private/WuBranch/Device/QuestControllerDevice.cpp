// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/QuestControllerDevice.h"
#include <InputMappingContext.h>
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include "InputActionValue.h"
#include <Kismet/KismetSystemLibrary.h>
//#include "Android/"

UQuestControllerDevice::UQuestControllerDevice()
{
	FString Path = "/Game/WuBranch/Input/QuestControllerInputMap";
	_mappingContext = LoadObject<UInputMappingContext>(nullptr, *Path);

	Path = "/Game/WuBranch/Input/Action/MoveAction";
	_moveAction = LoadObject<UInputAction>(nullptr, *Path);
}

void UQuestControllerDevice::Init()
{
	SetupKey();
}

void UQuestControllerDevice::SetupKey()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null!"));
		return;
	}

	// Add Input Mapping Context
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(_mappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		// 移動
		EnhancedInputComponent->BindAction(_moveAction, ETriggerEvent::Triggered, this, &UQuestControllerDevice::OnMove);
	}
	else
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void UQuestControllerDevice::OnMove(const FInputActionValue& Value)
{
	FVector2D InputVector = Value.Get<FVector2D>();

	FVector2D MoveVector(InputVector.Y, InputVector.X);
	UKismetSystemLibrary::PrintString(this, "quest controller input vector:" + MoveVector.ToString(), true, false, FColor::Green, 10.f);
	//UE_LOG(LogTemplateDevice, Log, TEXT("Move Value: %f, %f"), moveValue.X, moveValue.Y);

	// notify
	if (_onMoveEvent.IsBound())
		_onMoveEvent.Broadcast(MoveVector);
}
