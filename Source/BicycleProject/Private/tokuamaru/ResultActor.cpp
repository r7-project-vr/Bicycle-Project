// Fill out your copyright notice in the Description page of Project Settings.


#include "tokuamaru/ResultActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "tokuamaru/ResultWidget.h"

// Sets default values
AResultActor::AResultActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* scene = CreateDefaultSubobject<USceneComponent>(FName("Scene"));
	RootComponent = scene;
	AddInstanceComponent(RootComponent);

	_widgetResult = CreateDefaultSubobject<UWidgetComponent>(FName("widget"));
	_widgetResult->SetupAttachment(RootComponent);
	AddInstanceComponent(_widgetResult);

}

// Called when the game starts or when spawned
void AResultActor::BeginPlay()
{
	Super::BeginPlay();

	answerTrue = 1.0f;
	answerFalse = 0.0f;
	resultWidget = Cast<UResultWidget>(_widgetResult->GetWidget());
	resultWidget->SetRotation(answerTrue);
	resultWidget->SetRotationF(answerFalse);
	resultWidget->SetColorAndText(0.2f, 0.0f, 0.2f, 1.0f, FText::FromString(TEXT(" Auto Mode")));
	resultWidget->SetPercent(0.0f);

	//gamemode = GetWorld()->SpawnActor<AQuestionGameMode>(AQuestionGameMode::StaticClass());
	//if (!gamemode) {
	//	UE_LOG(LogTemp, Error, TEXT("gamemode NULLPtr!  ResultActor"));
	//}

	gamemode = Cast<AQuestionGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!gamemode) {
		UE_LOG(LogTemp, Error, TEXT("gamemode NULLPtr!  ResultActor"));
	}

	bikeComponent = FindComponentByClass<UBikeComponent>();
	if (!bikeComponent) {
	}
	else {
		this->autoplay = bikeComponent->GetIsAutoPlay();
	}
}

// Called every frame
void AResultActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (gamemode) {
	//	answerTrue = gamemode->GetCurrectNumber();
	//	answerFalse = gamemode->GetWrongNumber();
	//}
	if (answerTrue >= 0.1f) {
		answerTrue -= 0.001f;
	}

	if (answerFalse <= 0.9f) {
		answerFalse += 0.001f;
	}
	resultWidget->SetRotation(answerTrue);
	resultWidget->SetRotationF(answerFalse);

	float answerPercent = answerTrue / (answerTrue + answerFalse) * 100;

	resultWidget->SetPercent(answerPercent);

	if (autoplay) {
		resultWidget->SetColorAndText(0.2f, 0.0f, 0.2f, 1.0f, FText::FromString(TEXT(" Auto Mode")));
	}
	else {
		resultWidget->SetColorAndText(1.0f, 0.4f, 0.0f, 1.0f, FText::FromString(TEXT(" Manual Mode")));
	}
}

