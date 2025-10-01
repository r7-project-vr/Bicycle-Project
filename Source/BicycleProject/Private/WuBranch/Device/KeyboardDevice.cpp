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
	: Controller(nullptr)
{
	DefualtMap = nullptr;
	FString Path = "/Game/WuBranch/Input/KeyboardInputMap";
	DefualtMap = LoadObject<UInputMappingContext>(nullptr, *Path);

	MoveAction = nullptr;
	Path = "/Game/WuBranch/Input/Action/MoveAction";
	MoveAction = LoadObject<UInputAction>(nullptr, *Path);

	AnswerSelectMap = nullptr;
	Path = "/Game/WuBranch/Input/AnswerSelectMap";
	AnswerSelectMap = LoadObject<UInputMappingContext>(nullptr, *Path);

	SelectLeftAction = nullptr;
	Path = "/Game/WuBranch/Input/Action/SelectLeftAction";
	SelectLeftAction = LoadObject<UInputAction>(nullptr, *Path);

	SelectRightAction = nullptr;
	Path = "/Game/WuBranch/Input/Action/SelectRightAction";
	SelectRightAction = LoadObject<UInputAction>(nullptr, *Path);
}

void UKeyboardDevice::Init()
{
	Controller = GetWorld()->GetFirstPlayerController();
	EnableMoveAction_Implementation();
	SetupAction();
}

bool UKeyboardDevice::Connect()
{
	// デフォルトとして使う、つまりunrealのEnhancedInputを使う
	State = EDeviceConnectType::Connected;
	return true;
}

bool UKeyboardDevice::Disconnect()
{
	// 実際キーボードやゲームパッドなど線を繋がっているままゲーム内でリンクを断つ方法はわからないので(ケーブルを抜くとかしか)
	// とりあえず、マッピングしているMappingContextを全部クリアする
	if (!Controller)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null when enable the action of selecting answer", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null when enable default action!"));
		return false;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
	}
	State = EDeviceConnectType::UnConnected;
	return true;
}

void UKeyboardDevice::EnableMoveAction_Implementation()
{
	if (!Controller)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null when enable the action of selecting answer", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null when enable default action!"));
		return;
	}

	if (!DefualtMap)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("mapping Context is null!"));
		return;
	}

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer()))
	{
		if (!Subsystem->HasMappingContext(DefualtMap))
		{
			Subsystem->AddMappingContext(DefualtMap, 0);
		}
	}
}

void UKeyboardDevice::DisableMoveAction_Implementation()
{
	if (!Controller)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null when enable the action of selecting answer", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null when enable default action!"));
		return;
	}

	if (!DefualtMap)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("mapping Context is null!"));
		return;
	}

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer()))
	{
		if (Subsystem->HasMappingContext(DefualtMap))
		{
			Subsystem->RemoveMappingContext(DefualtMap);
		}
	}
}

void UKeyboardDevice::EnableSelectAnswerAction_Implementation()
{
	if (!Controller)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null when enable the action of selecting answer", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null when enable the action of selecting answer!"));
		return;
	}

	if (!AnswerSelectMap)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("answer select mapping Context is null!"));
		return;
	}

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer()))
	{
		if (!Subsystem->HasMappingContext(AnswerSelectMap))
		{
			Subsystem->AddMappingContext(AnswerSelectMap, 0);
		}
	}
}

void UKeyboardDevice::DisableSelectAnswerAction_Implementation()
{
	if (!Controller)
	{
		UKismetSystemLibrary::PrintString(this, "player controller is null when diable the action of selecting answer", true, false, FColor::Red, 10.f);
		UE_LOG(LogTemplateDevice, Error, TEXT("Player controller is null!"));
		return;
	}

	// Remove Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer()))
	{
		if (Subsystem->HasMappingContext(AnswerSelectMap))
		{
			Subsystem->RemoveMappingContext(AnswerSelectMap);
		}
	}
}

void UKeyboardDevice::SetupAction()
{
	if (!MoveAction)
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("move Action is null!"));
		return;
	}

	// Set up action bindings
	// ここはプレイヤコントローラのInputComponentではなく、プレイヤのアクターのInputComponentを利用する
	// そうすると、Unreal本来の設計通り、PawnのEnableInput、DisableInputでコントロールできる
	// *PlayerControllerとPawn両方ともActorの派生クラスなので、両方ともInputComponent持っています。ここは間違われやすい。
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(Controller->GetPawn()->InputComponent))
	{
		// 移動
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UKeyboardDevice::OnMove);

		// 左の答えを選択するアクション
		EnhancedInputComponent->BindAction(SelectLeftAction, ETriggerEvent::Started, this, &UKeyboardDevice::OnSelectLeftAnswer);

		// 右の答えを選択するアクション
		EnhancedInputComponent->BindAction(SelectRightAction, ETriggerEvent::Started, this, &UKeyboardDevice::OnSelectRightAnswer);
	}
	else
	{
		UE_LOG(LogTemplateDevice, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void UKeyboardDevice::OnMove(const FInputActionValue& Value)
{
	FVector2D InputVector = Value.Get<FVector2D>();

	// 左右の入力を一旦無視
	//FVector2D moveVector(inputVector.Y, inputVector.X);
	FVector2D MoveVector(InputVector.Y, 0);

	// notify
	if(OnMoveEvent.IsBound())
		OnMoveEvent.Broadcast(MoveVector);
}

void UKeyboardDevice::OnSelectLeftAnswer()
{
	// notify
	if (OnSelectLeftEvent.IsBound())
		OnSelectLeftEvent.Broadcast();
}

void UKeyboardDevice::OnSelectRightAnswer()
{
	// notify
	if (OnSelectRightEvent.IsBound())
		OnSelectRightEvent.Broadcast();
}
