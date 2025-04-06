// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/KeyboardDevice.h"
#include <InputMappingContext.h>
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include "InputActionValue.h"
#include <Kismet/KismetSystemLibrary.h>
//#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(LogTemplateDevice);

UKeyboardDevice::UKeyboardDevice()
{
	_defaultMap = nullptr;
	FString path = "/Game/WuBranch/Input/KeyboardInputMap";
	_defaultMap = LoadObject<UInputMappingContext>(nullptr, *path);

	_moveAction = nullptr;
	path = "/Game/WuBranch/Input/Action/MoveAction";
	_moveAction = LoadObject<UInputAction>(nullptr, *path);

	_answerSelectMap = nullptr;
	path = "/Game/WuBranch/Input/AnswerSelectMap";
	_answerSelectMap = LoadObject<UInputMappingContext>(nullptr, *path);

	_selectLeftAction = nullptr;
	path = "/Game/WuBranch/Input/Action/SelectLeftAction";
	_selectLeftAction = LoadObject<UInputAction>(nullptr, *path);

	_selectRightAction = nullptr;
	path = "/Game/WuBranch/Input/Action/SelectRightAction";
	_selectRightAction = LoadObject<UInputAction>(nullptr, *path);
}

void UKeyboardDevice::Init()
{
	_controller = GetWorld()->GetFirstPlayerController();
	SetupKey();
}

void UKeyboardDevice::EnableSelectAnswerActions_Implementation()
{
	if (!_controller)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null when enable the action of selecting answer", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null when enable the action of selecting answer!"));
		return;
	}

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_controller->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(_answerSelectMap, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(_controller->InputComponent))
	{
		// 左の答えを選択するアクション
		_selectLeftActionID = EnhancedInputComponent->BindAction(_selectLeftAction, ETriggerEvent::Started, this, &UKeyboardDevice::OnSelectLeftAnswer).GetHandle();

		// 右の答えを選択するアクション
		_selectRightActionID = EnhancedInputComponent->BindAction(_selectRightAction, ETriggerEvent::Started, this, &UKeyboardDevice::OnSelectRightAnswer).GetHandle();
	}
	else
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void UKeyboardDevice::DisableSelectAnswerActions_Implementation()
{
	if (!_controller)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null when diable the action of selecting answer", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null!"));
		return;
	}

	// Remove Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_controller->GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(_answerSelectMap);
	}

	// バインドされた関数を解除
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(_controller->InputComponent))
	{
		// 左の答えを選択するアクション
		EnhancedInputComponent->RemoveActionBindingForHandle(_selectLeftActionID);

		// 右の答えを選択するアクション
		EnhancedInputComponent->RemoveActionBindingForHandle(_selectRightActionID);
	}
}

void UKeyboardDevice::SetupKey()
{
	if (!_controller)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null when enable default action", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null when enable default action!"));
		return;
	}

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_controller->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(_defaultMap, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(_controller->InputComponent))
	{
		// 移動
		EnhancedInputComponent->BindAction(_moveAction, ETriggerEvent::Triggered, this, &UKeyboardDevice::OnMove);
	}
	else
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void UKeyboardDevice::OnMove(const FInputActionValue& Value)
{
	FVector2D inputVector = Value.Get<FVector2D>();

	FVector2D moveVector(inputVector.Y, inputVector.X);

	// notify
	if(_onMoveEvent.IsBound())
		_onMoveEvent.Broadcast(moveVector);
}

void UKeyboardDevice::OnSelectLeftAnswer()
{
	// notify
	if (_onSelectLeftEvent.IsBound())
		_onSelectLeftEvent.Broadcast();
}

void UKeyboardDevice::OnSelectRightAnswer()
{
	// notify
	if (_onSelectRightEvent.IsBound())
		_onSelectRightEvent.Broadcast();
}
