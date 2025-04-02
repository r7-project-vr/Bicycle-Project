// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/QuestControllerDevice.h"
#include <InputMappingContext.h>
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include "InputActionValue.h"
#include <Kismet/KismetSystemLibrary.h>

UQuestControllerDevice::UQuestControllerDevice()
{
	FString path = "/Game/WuBranch/Input/QuestControllerInputMap";
	_mappingContext = LoadObject<UInputMappingContext>(nullptr, *path);

	path = "/Game/WuBranch/Input/Action/MoveAction";
	_moveAction = LoadObject<UInputAction>(nullptr, *path);
}

void UQuestControllerDevice::Init()
{
	SetupKey();
}

void UQuestControllerDevice::SetupKey()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (!playerController)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null!"));
		return;
	}

	// Add Input Mapping Context
	if (playerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(_mappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(playerController->InputComponent))
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
	FVector2D inputVector = Value.Get<FVector2D>();

	FVector2D moveVector(inputVector.Y, inputVector.X);
	UKismetSystemLibrary::PrintString(this, "quest controller input vector:" + moveVector.ToString(), true, false, FColor::Green, 10.f);
	//UE_LOG(LogTemplateDevice, Log, TEXT("Move Value: %f, %f"), moveValue.X, moveValue.Y);

	// notify
	if (_onMoveEvent.IsBound())
		_onMoveEvent.Broadcast(moveVector);
}
