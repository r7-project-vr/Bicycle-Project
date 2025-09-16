// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/WildAnimalManagerComponent.h"
#include "Components/BoxComponent.h"
#include "WuBranch/Actor/WildAnimal.h"
#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UWildAnimalManagerComponent::UWildAnimalManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
	for(int Index = 0; Index < 5; Index++)
	{
		UBoxComponent* BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName(*FString::Printf(TEXT("Wild Animal Spawn Location%d"), Index)));
		if(BoxComponent)
		{
			BoxComponent->SetupAttachment(this);
			WildAnimalSpawnLocations.Add(BoxComponent);
		}
	}
}

// Called when the game starts
void UWildAnimalManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
//void UWildAnimalManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UWildAnimalManagerComponent::SetSeed(int Seed)
{
	RandomStream.Initialize(Seed);
}

void UWildAnimalManagerComponent::StartSpawnAnimal()
{
	CaculateTotalProbility();
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	for(int Index = 0; Index < WildAnimalSpawnLocations.Num(); Index++)
	{
		if(WildAnimalSpawnLocations[Index])
		{
			FVector Location = WildAnimalSpawnLocations[Index]->GetComponentLocation();
			FRotator Rotation = WildAnimalSpawnLocations[Index]->GetComponentRotation();
			CreateAnimal(Character, DecideAnimal(), Location, Rotation);
		}
	}
}

void UWildAnimalManagerComponent::DestroyAllAnimals()
{
	if (SpawnedAnimals.Num() <= 0)
		return;

	while (SpawnedAnimals.Num() > 0)
	{
		AWildAnimal* Animal = SpawnedAnimals.Pop();
		Animal->Destroy();
	}
}

void UWildAnimalManagerComponent::CaculateTotalProbility()
{
	// 合計確率を初期化
	TotalProbility = 0.0f;

	// Mapに内容がない場合
	if (WildAnimalTypes.Num() == 0)
		return;

	// 合計を計算
	TArray<int> Probilities;
	WildAnimalTypes.GenerateValueArray(Probilities);
	for (const int& Probility : Probilities)
	{
		TotalProbility += Probility;
	}
}

void UWildAnimalManagerComponent::CreateAnimal(ACharacter* Character, TSubclassOf<AWildAnimal> Target, FVector Location, FRotator Rotation)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("Create Environmental Object Failed"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Location: %s, Rotation: "), *Location.ToString(), *Rotation.ToString()));

	AWildAnimal* Animal = GetWorld()->SpawnActor<AWildAnimal>(Target, Location, Rotation);
	if (Animal)
	{
		Animal->Init(Character, nullptr);
		SpawnedAnimals.Add(Animal);
	}
}

TSubclassOf<AWildAnimal> UWildAnimalManagerComponent::DecideAnimal()
{
	// １から合計確率までランダムで数値をゲット
	int Target = RandomStream.RandRange(1, TotalProbility);
	// 
	auto ObjectsArray = WildAnimalTypes.Array();
	int Sum = 0;
	TSubclassOf<AWildAnimal> Object = nullptr;
	for (const auto& ObjectType : ObjectsArray)
	{
		Sum += ObjectType.Value;
		if (Sum >= Target)
		{
			Object = ObjectType.Key;
			break;
		}
	}
	return Object;
}
