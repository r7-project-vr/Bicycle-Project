// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Device/KeyboardDevice.h"
#include <InputMappingContext.h>
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include "InputActionValue.h"
#include <Kismet/KismetSystemLibrary.h>
#include <WuBranch/BikePlayerController.h>

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
	EnableDefaultActions_Implementation();
	SetupAction();
}

void UKeyboardDevice::EnableDefaultActions_Implementation()
{
	if (!_controller)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null when enable the action of selecting answer", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null when enable default action!"));
		return;
	}

	if (!_defaultMap)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("mapping Context is null!"));
		return;
	}

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_controller->GetLocalPlayer()))
	{
		if (!Subsystem->HasMappingContext(_defaultMap))
		{
			Subsystem->AddMappingContext(_defaultMap, 0);
		}
	}	
}

void UKeyboardDevice::DisableDefaultActions_Implementation()
{
	if (!_controller)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null when enable the action of selecting answer", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null when enable default action!"));
		return;
	}

	if (!_defaultMap)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("mapping Context is null!"));
		return;
	}

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_controller->GetLocalPlayer()))
	{
		if (Subsystem->HasMappingContext(_defaultMap))
		{
			Subsystem->RemoveMappingContext(_defaultMap);
		}
	}
}

void UKeyboardDevice::EnableSelectAnswerActions_Implementation()
{
	if (!_controller)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null when enable the action of selecting answer", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null when enable the action of selecting answer!"));
		return;
	}

	if (!_answerSelectMap)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("answer select mapping Context is null!"));
		return;
	}

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_controller->GetLocalPlayer()))
	{
		if (!Subsystem->HasMappingContext(_answerSelectMap))
		{
			Subsystem->AddMappingContext(_answerSelectMap, 0);
		}
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
		if (Subsystem->HasMappingContext(_answerSelectMap))
		{
			Subsystem->RemoveMappingContext(_answerSelectMap);
		}
	}
}

void UKeyboardDevice::SetupAction()
{
	if (!_moveAction)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("move Action is null!"));
		return;
	}

	// Set up action bindings
	// ここはプレイヤコントローラのInputComponentではなく、プレイヤのアクターのInputComponentを利用する
	// そうすると、Unreal本来の設計通り、PawnのEnableInput、DisableInputでコントロールできる
	// *PlayerControllerとPawn両方ともActorの派生クラスなので、両方ともInputComponent持っています。ここは間違われやすい。
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(_controller->GetPawn()->InputComponent))
	{
		// 移動
		EnhancedInputComponent->BindAction(_moveAction, ETriggerEvent::Triggered, this, &UKeyboardDevice::OnMove);

		// 左の答えを選択するアクション
		EnhancedInputComponent->BindAction(_selectLeftAction, ETriggerEvent::Started, this, &UKeyboardDevice::OnSelectLeftAnswer);

		// 右の答えを選択するアクション
		EnhancedInputComponent->BindAction(_selectRightAction, ETriggerEvent::Started, this, &UKeyboardDevice::OnSelectRightAnswer);
	}
	else
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void UKeyboardDevice::OnMove(const FInputActionValue& Value)
{
	FVector2D inputVector = Value.Get<FVector2D>();

	// 左右の入力を一旦無視
	//FVector2D moveVector(inputVector.Y, inputVector.X);
	FVector2D moveVector(inputVector.Y, 0);

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
