// Fill out your copyright notice in the Description page of Project Settings.


#include "tokuamaru/ResultSceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WuBranch/QuestionGameMode.h"
//アクターに直接ウィジェットを追加してもらってそれを見つける感じ

// Sets default values for this component's properties
UResultSceneComponent::UResultSceneComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);
	bAutoActivate = true;

	//USceneComponent* scene = CreateDefaultSubobject<USceneComponent>(FName("Scene"));
	//RootComponent = scene;
	//AddInstanceComponent(RootComponent);

	//_widgetResult = CreateDefaultSubobject<UWidgetComponent>(FName("widget"));
	//_widgetResult->SetupAttachment(RootComponent);
	//AddInstanceComponent(_widgetResult);

	// ...
}


// Called when the game starts
void UResultSceneComponent::BeginPlay()
{
	Super::BeginPlay();

	// 2025.08.25 ウー start
	//answerTrue = 1.0f;
	//answerFalse = 0.0f;
	//this->autoplay = false;
	// 2025.08.25 ウー end


	gamemode = Cast<AQuestionGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!gamemode) {
		UE_LOG(LogTemp, Error, TEXT("gamemode NULLPtr!  ResultActor"));
		return;
	}
	else {
		// 2025.08.25 ウー start
		//UE_LOG(LogTemp, Error, TEXT("gamemode true!  ResultActor"));
		gamemode->OnUpdateAnswerUIDelegate.AddDynamic(this, &UResultSceneComponent::UpdateAnswer);
		// 2025.08.25 ウー end
	}

	TArray<USceneComponent*> Children;
	GetChildrenComponents(true, Children);
	for (USceneComponent* Child : Children)
	{
		if (!IsValid(Child)) continue;

		widgetComp = Cast<UWidgetComponent>(Child);
		if (widgetComp)
		{
			//UE_LOG(LogTemp, Error, TEXT("widgetComp true!"));
			widgetComp->InitWidget();
			widget = widgetComp->GetUserWidgetObject();
			if (widget)
			{
				//UE_LOG(LogTemp, Error, TEXT("widget true!"));
				resultWidget= Cast<UResultWidget>(widget);
				if (resultWidget)
				{
					//UE_LOG(LogTemp, Error, TEXT("resultWidget true!  ResultActor"));
					resultWidget->SetRotation(gamemode->GetCurrectNumber());
					resultWidget->SetRotationF(gamemode->GetWrongNumber());
					resultWidget->SetColorAndText(0.2f, 0.0f, 0.2f, 1.0f, FText::FromString(TEXT(" Auto Mode")));
					resultWidget->SetPercent(0.0f);
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("resultWidget NULLPtr!"));
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("widget NULLPtr!"));
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("widgetComp NULLPtr!  ResultActor"));
		}
	}

	bikeComponent = GetOwner()->FindComponentByClass<UBikeComponent>();
	if (bikeComponent == NULL) {
		UE_LOG(LogTemp, Error, TEXT("bikeComponent NULLPtr!  BikeComponent"));
	}
	else {
		// 2025.08.25 ウー start
		bikeComponent->OnUpdateAutoPlayEvent.AddDynamic(this, &UResultSceneComponent::UpdateAutoPlay);
		//this->autoplay = bikeComponent->GetIsAutoPlay();
		UpdateAutoPlay(bikeComponent->GetIsAutoPlay());	
		// 2025.08.25 ウー end
	}

	// ...
	
}

// Called every frame
void UResultSceneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 2025.08.25 ウー start
	//if (gamemode) {
	////UE_LOG(LogTemp, Error, TEXT("SetAC"));
	//answerTrue = gamemode->GetCurrectNumber();
	//answerFalse = gamemode->GetWrongNumber();
 //   }

	/*if (bikeComponent) {
		this->autoplay = bikeComponent->GetIsAutoPlay();
	}*/

	//if (resultWidget) {
	//	//UE_LOG(LogTemp, Error, TEXT("SetRotation"));
	//	resultWidget->SetRotation(answerTrue);
	//	resultWidget->SetRotationF(answerFalse);
	//	float answerPercent = ((answerTrue + answerFalse) > 0.0f) ? answerTrue / (answerTrue + answerFalse) * 100 : 0.0f;
	//	resultWidget->SetPercent(answerPercent);
	//	if (autoplay) {
	//		resultWidget->SetColorAndText(0.2f, 0.0f, 0.2f, 1.0f, FText::FromString(TEXT(" Auto Mode")));
	//		/*UE_LOG(LogTemp, Error, TEXT("auto"));*/
	//	}
	//	else {
	//		resultWidget->SetColorAndText(1.0f, 0.4f, 0.0f, 1.0f, FText::FromString(TEXT(" Manual Mode")));
	//		//UE_LOG(LogTemp, Error, TEXT("false"));
	//	}
	//}
	// 2025.08.25 ウー end
	// ...
}

// 2025.08.25 ウー start
void UResultSceneComponent::UpdateAnswer(int Correct, int Wrong)
{
	if (resultWidget) {
		//UE_LOG(LogTemp, Error, TEXT("SetRotation"));
		resultWidget->SetRotation((float)Correct / (float)(Correct + Wrong));
		resultWidget->SetRotationF((float)Wrong / (float)(Correct + Wrong));
		float AnswerPercent = ((Correct + Wrong) > 0.0f) ? (float)Correct / (float)(Correct + Wrong) * 100 : 0.0f;
		resultWidget->SetPercent(AnswerPercent);
	}
}

void UResultSceneComponent::UpdateAutoPlay(bool IsAutoPlay)
{
	if (resultWidget) 
	{
		if (IsAutoPlay) {
			resultWidget->SetColorAndText(0.2f, 0.0f, 0.2f, 1.0f, FText::FromString(TEXT(" Auto Mode")));
			/*UE_LOG(LogTemp, Error, TEXT("auto"));*/
		}
		else {
			resultWidget->SetColorAndText(1.0f, 0.4f, 0.0f, 1.0f, FText::FromString(TEXT(" Manual Mode")));
			//UE_LOG(LogTemp, Error, TEXT("false"));
		}
	}
}

// 2025.08.25 ウー end