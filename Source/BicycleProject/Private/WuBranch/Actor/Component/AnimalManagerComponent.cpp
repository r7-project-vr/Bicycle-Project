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
		// TSubclassOfからカプセルの高さの半分をゲット
		float CapsuleHalfHeight = 0.0f;
		float CapsuleRadius = 0.f;
		if (AnimalClass)
		{
			AAnimal* DefaultAnimal = AnimalClass->GetDefaultObject<AAnimal>();
			if (DefaultAnimal && DefaultAnimal->GetCapsuleComponent())
			{
				CapsuleHalfHeight = DefaultAnimal->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				CapsuleRadius = DefaultAnimal->GetCapsuleComponent()->GetScaledCapsuleRadius();
			}
		}

		// おいていける位置を探す
		FVector GroundLocation;
		bool bIsValidLocation = false;
		do {
			FVector NewLocation = GetRandomLocationNearPlayer();
			bIsValidLocation = CheckLocation(CapsuleRadius, NewLocation, GroundLocation);
		} while (!bIsValidLocation);

		// 高さの調整
		FVector SpawnLocation = GroundLocation + FVector(0, 0, CapsuleHalfHeight);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AAnimal* Animal = GetWorld()->SpawnActor<AAnimal>(AnimalClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
				
		if (Animal)
		{
			// 設定
			Animal->Init(Target, this);
			FVector Offset = GroundLocation - Target->GetActorLocation();
			Animal->ChangeOffset(FVector(Offset.X, Offset.Y, 0));
			FollowingAnimals.Add(Animal);
		}
	}
}

FVector UAnimalManagerComponent::GetRandomLocationNearPlayer()
{
	// 円形
	// 今プレイヤーの正面は0度になっている
	// ランダムな角度(30度 ~ 75度, 285度 ~ 330度)
	float Angles[4] = { (float)1 / 6, (float)5 / 12, (float)19 / 12, (float)11 / 6 };
	// 左右をランダムで決める、左：［0, 0.5), 右: [0.5, 1)
	int Side = FMath::RoundToInt(FMath::SRand());
	float StartAngle = Angles[Side * 2];
	float EndAngle = Angles[Side * 2 + 1];
	float Angle = RandomStream.FRandRange(StartAngle * PI, EndAngle * PI);

	// ランダムな距離（200～800, 道路の幅さ）、均等分布のため sqrt を使う
	float Distance = FMath::Sqrt(RandomStream.FRandRange(0.0f, 1.0f)) * 800;
	// 最小距離200cm、最大800cm
	Distance = Distance < 200.f ? 200.f : Distance;

	float X = FMath::Cos(Angle) * Distance;
	float Y = FMath::Sin(Angle) * Distance;

	// 対象を中心にした
	FVector Location = Target->GetActorLocation();
	Location.X += X;
	Location.Y += Y;
	return Location;
}

bool UAnimalManagerComponent::CheckLocation(float CollisionRadius, const FVector& Location, FVector& OGroundLocation)
{
	FHitResult HitResult;
	FVector Start = Location + FVector(0, 0, 2000);
	FVector End = Location + FVector(0, 0, -2000);

	// 円形の射線
	FCollisionShape Sphere = FCollisionShape::MakeSphere(CollisionRadius);
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
