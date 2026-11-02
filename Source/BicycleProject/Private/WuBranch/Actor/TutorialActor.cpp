// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/TutorialActor.h"
#include "Components/WidgetComponent.h"
#include "WuBranch/UI/TutorialTextPanelWidget.h"
#include "WuBranch/System/Tutorialsystem.h"

// Sets default values
ATutorialActor::ATutorialActor()
	: TutorialType(ETutorialType::Controller)
	, CurrentStep(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATutorialActor::BeginPlay()
{
	Super::BeginPlay();

	Messages.Empty();
	Initialize();
	InitMessages();
	ResetStep();

	//FinishTutorial();
}

UTutorialTextPanelWidget* ATutorialActor::GetTutorialTextPanelWidget(UWidgetComponent* Widget)
{
	if (!Widget)
		return nullptr;

	UTutorialTextPanelWidget* TutorialTextPanelWidget = Cast<UTutorialTextPanelWidget>(Widget->GetUserWidgetObject());
	return TutorialTextPanelWidget;
}

void ATutorialActor::GotoNextStep()
{
	// 今が最後のメッセージの場合、チュートリアルを終了する
	if (CurrentStep == Messages.Num() - 1)
	{
		if (UTutorialsystem* TutorialSystem = GetGameInstance()->GetSubsystem<UTutorialsystem>())
		{
			TutorialSystem->FinishTutorial(TutorialType);
		}
		return;
	}

	CurrentStep = FMath::Clamp(CurrentStep + 1, 0, Messages.Num() - 1);
	HandleStepTasks();
}

// Called every frame
//void ATutorialActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void ATutorialActor::StartTutorial_Implementation()
{
	SetActorHiddenInGame(false);
	ResetStep();
}

void ATutorialActor::FinishTutorial_Implementation()
{
	SetActorHiddenInGame(true);
}

ETutorialType ATutorialActor::GetTutorialType() const
{
	return TutorialType;
}

FString ATutorialActor::GetCurrentMessage()
{
	if (Messages.IsEmpty() || CurrentStep >= Messages.Num())
		return FString();

	return Messages[CurrentStep];
}

void ATutorialActor::ResetStep()
{
	CurrentStep = 0;
	HandleStepTasks();
}
