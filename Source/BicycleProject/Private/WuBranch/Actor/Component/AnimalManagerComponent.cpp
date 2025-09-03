// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/AnimalManagerComponent.h"
#include "GameFramework/Character.h"
#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Actor/Animal.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UAnimalManagerComponent::UAnimalManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UAnimalManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	RandomStream.Initialize(FDateTime::Now().GetTicks());
	SetTarget();
	UMyGameInstance* GameInstance = GetWorld()->GetGameInstance<UMyGameInstance>();
	if (GameInstance)
	{
		ArrangeAroundTarget(GameInstance->GetAnimals());
	}
}

// Called every frame
//void UAnimalManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UAnimalManagerComponent::ReduceAnimal(AAnimal* Animal)
{
	if (FollowingAnimals.Num() <= 0 || !FollowingAnimals.Contains(Animal))
		return;

	FollowingAnimals.Remove(Animal);

	UMyGameInstance* GameInstance = GetWorld()->GetGameInstance<UMyGameInstance>();
	GameInstance->RemoveAnimal(Animal->StaticClass());
}

void UAnimalManagerComponent::SetTarget()
{
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		Target = Character;
	}
}

void UAnimalManagerComponent::ArrangeAroundTarget(TArray<TSubclassOf<AAnimal>> Animals)
{
	for (TSubclassOf<AAnimal> AnimalClass : Animals)
	{
		// おいていける位置を探す
		FVector NewLocation;
		do {
			NewLocation = GetRandomLocationNearPlayer();
		} while (!CheckLocation(NewLocation));

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AAnimal* Animal = GetWorld()->SpawnActor<AAnimal>(AnimalClass, NewLocation, FRotator::ZeroRotator, SpawnParams);
		
		// 設定
		if (Animal)
		{
			Animal->SetTarget(Target);
			FollowingAnimals.Add(Animal);
		}
	}
}

FVector UAnimalManagerComponent::GetRandomLocationNearPlayer()
{
	// 円形
	// ランダムな角度（0～2π）
	float Angle = RandomStream.FRandRange(0.f, 2 * PI);

	// ランダムな距離（0～半径）、均等分布のため sqrt を使う
	float Distance = FMath::Sqrt(RandomStream.FRand()) * Radius;

	float X = FMath::Cos(Angle) * Distance;
	float Y = FMath::Sin(Angle) * Distance;

	// 対象を中心にした
	FVector Location = Target->GetActorLocation();
	Location.X += X;
	Location.Y += Y;
	return Location;
}

bool UAnimalManagerComponent::CheckLocation(FVector Location)
{
	FHitResult HitResult;
	FVector Start = Location + FVector(0, 0, 2000);
	FVector End = Location + FVector(0, 0, -2000);

	// 円形の射線
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_Visibility, Sphere);

	if (!bHit)
		return false;

	// 地面の確認
	if (!HitResult.GetActor()->ActorHasTag("Tile"))
		return false;

	return true;
}
