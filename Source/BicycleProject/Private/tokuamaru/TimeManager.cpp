// Fill out your copyright notice in the Description page of Project Settings.


#include "tokuamaru/TimeManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "tokuamaru/TimerWidget.h"
#include "tokuamaru/QuestionManager.h"
#include "Kismet/KismetSystemLibrary.h"

class UInputMappingContext;

// Sets default values
ATimeManager::ATimeManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* scene = CreateDefaultSubobject<USceneComponent>(FName("Scene"));
	RootComponent = scene;
	AddInstanceComponent(RootComponent);

	_widgetT = CreateDefaultSubobject<UWidgetComponent>(FName("widget"));
	_widgetT->SetupAttachment(RootComponent);
	AddInstanceComponent(_widgetT);


	//UQuestionManager* QuestionInstance;
	//QuestionInstance = ObjectInitializer.CreateDefaultSubobject<UQuestionManager>(this,TEXT("hoge"));

	//timeEnd.AddDynamic(QuestionInstance, &UQuestionManager::printtest);

}

// Called when the game starts or when spawned
void ATimeManager::BeginPlay()
{
	Super::BeginPlay();

	//UObjectの関数をバインドしている。もしアクター
	UTimerWidget* WidgetInstance = NewObject<UTimerWidget>(this, UTimerWidget::StaticClass(), TEXT("hoge"));
	if (WidgetInstance)
	{
		timeEnd.AddDynamic(WidgetInstance, &UTimerWidget::printet);
	}

	timer = 5.0f;
	TimerW = Cast<UTimerWidget>(_widgetT->GetWidget());
	stoper = false;
	soundPlay = false;

	SetupInput();

}

// Called every frame
void ATimeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (stoper) {
		return;
	}

	timer -= DeltaTime;

	if (!TimerW) {
		UE_LOG(LogTemp, Error, TEXT("TimerW NULLPtr!"));
	}
	else {
		if (timer >= 0) {
			TimerW->SetTimer(timer);
		}
		else if (timer < 0) {
			timer = 0;
			FVector timerVec = GetActorLocation();
			TimerW->FinishTimer(timerVec);

			timeEnd.Broadcast();

			stoper = true;

		}
	}

	if (timer <= 6) {
		if(!soundPlay)
		SoundPlay();
		soundPlay = true;
	}
}

void ATimeManager::printst()
{
	UE_LOG(LogTemp, Error, TEXT("w"));
}

void ATimeManager::SetupInput()
{
	UE_LOG(LogTemp, Error, TEXT("nine"));

	if (!DefaultMappingContext) {
		UE_LOG(LogTemp, Error, TEXT("null"));
		return;
	}

	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (APlayerController* PlayerController = Cast<APlayerController>(controller))
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UE_LOG(LogTemp, Error, TEXT("one"));
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(controller->GetPawn()->InputComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("two"));
		// Moving
		EnhancedInputComponent->BindAction(ActionInput, ETriggerEvent::Started, this, &ATimeManager::TimerStop);
	}
}

void ATimeManager::TimerStop()
{
	UE_LOG(LogTemp, Error, TEXT("op"));
	if (!stoper) {
		stoper = true;
	}
	else {
		stoper = false;
	}

}

void ATimeManager::SoundPlay() {
	if (!SoundToPlay) {
		UE_LOG(LogTemp, Error, TEXT("SoundBase NULL"));
	}
	else {
		UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation());
	}
}
