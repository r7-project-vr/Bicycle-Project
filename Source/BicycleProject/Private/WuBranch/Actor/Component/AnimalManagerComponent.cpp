// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/AnimalManagerComponent.h"
#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Actor/Animal.h"
#include "Components/CapsuleComponent.h"
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
		FVector GroundLocation;
		bool bIsValidLocation = false;
		do {
			FVector NewLocation = GetRandomLocationNearPlayer();
			bIsValidLocation = CheckLocation(NewLocation, GroundLocation);
		} while (!bIsValidLocation);

		// 高さの調整、TSubclassOfからカプセルの高さの半分をゲット
		float CapsuleHalfHeight = 0.0f;
		if (AnimalClass)
		{
			AAnimal* DefaultAnimal = AnimalClass->GetDefaultObject<AAnimal>();
			if (DefaultAnimal && DefaultAnimal->GetCapsuleComponent())
			{
				CapsuleHalfHeight = DefaultAnimal->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
			}
		}

		FVector SpawnLocation = GroundLocation + FVector(0, 0, CapsuleHalfHeight);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AAnimal* Animal = GetWorld()->SpawnActor<AAnimal>(AnimalClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
				
		if (Animal)
		{
			// 設定
			Animal->Init(Target, this);
			FollowingAnimals.Add(Animal);
		}
	}
}

FVector UAnimalManagerComponent::GetRandomLocationNearPlayer()
{
	// 円形
	// 今プレイヤーの正面は0度になっている
	// ランダムな角度(30度 ~ 330度)
	//float Angle = RandomStream.FRandRange(-(float)4 / 3 * PI, (float)2 / 3 * PI);
	float Angle = RandomStream.FRandRange((float)1 / 4 * PI, (float)7 / 4 * PI);

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

bool UAnimalManagerComponent::CheckLocation(FVector Location, FVector& OGroundLocation)
{
	FHitResult HitResult;
	FVector Start = Location + FVector(0, 0, 2000);
	FVector End = Location + FVector(0, 0, -2000);

	// 円形の射線
	FCollisionShape Sphere = FCollisionShape::MakeSphere(100.f);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	bool bHit = GetWorld()->SweepSingleByObjectType(HitResult, Start, End, FQuat::Identity, ObjectQueryParams, Sphere);

	if (!bHit)
		return false;

	// 地面の確認
	if (!HitResult.GetActor()->ActorHasTag("Tile"))
		return false;

	OGroundLocation = HitResult.ImpactPoint;
	return true;
}
