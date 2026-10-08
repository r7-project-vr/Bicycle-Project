// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Actor/Component/AnimalManagerComponent.h"
#include "WuBranch/MyGameInstance.h"
#include "WuBranch/Actor/Pet.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include <Engine/AssetManager.h>
#include <WuBranch/DataAssets/AnimalDataAsset.h>
#include "WuBranch/Bike/BikeComponent.h"

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

	// 漕ぎ過ぎ(赤ゲージ)になったら動物を1匹ずつ振り切る
	if (UBikeComponent* Bike = GetOwner()->FindComponentByClass<UBikeComponent>())
	{
		Bike->OnUpdatePenaltyEvent.AddDynamic(this, &UAnimalManagerComponent::OnPenaltyUpdated);
	}

	TArray<FPrimaryAssetId> AnimalIDs = GetAllAnimalID();
	LoadedAnimalCount = AnimalIDs.Num();
	for (const FPrimaryAssetId& ID : AnimalIDs)
	{
		UAssetManager::Get().LoadPrimaryAsset(ID, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UAnimalManagerComponent::OnAnimalLoaded, ID));
	}

//	UMyGameInstance* GameInstance = GetWorld()->GetGameInstance<UMyGameInstance>();
//	if (GameInstance)
//	{
//		// <動物ID, 数>
//		TArray<TSubclassOf<AAnimal>> Animals = GameInstance->GetAnimals();
//		ArrangeAroundTarget(Animals);
//	}
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

	// この走行の間だけ外す、持っている動物(セーブ)は減らさないので次のゲームでは戻ってくる
	FollowingAnimals.Remove(Animal);

	/*UMyGameInstance* GameInstance = GetWorld()->GetGameInstance<UMyGameInstance>();
	GameInstance->RemoveAnimal(Animal->GetMyID());*/
}

void UAnimalManagerComponent::OnPenaltyUpdated(bool bIsPenalty)
{
	if (bIsPenalty)
	{
		LeaveOneAnimal();
	}
}

void UAnimalManagerComponent::LeaveOneAnimal()
{
	if (!Target)
		return;

	// 一番後ろ(プレイヤーの進行方向で一番遅れている)の動物が振り切られる
	FVector TargetLocation = Target->GetActorLocation();
	FVector TargetForward = Target->GetActorForwardVector();
	APet* LastPet = nullptr;
	float LastForwardDistance = TNumericLimits<float>::Max();
	for (AAnimal* Animal : FollowingAnimals)
	{
		APet* Pet = Cast<APet>(Animal);
		if (!IsValid(Pet) || !Pet->IsFollowing())
			continue;

		float ForwardDistance = FVector::DotProduct(Pet->GetActorLocation() - TargetLocation, TargetForward);
		if (ForwardDistance < LastForwardDistance)
		{
			LastForwardDistance = ForwardDistance;
			LastPet = Pet;
		}
	}

	if (LastPet)
	{
		LastPet->LeaveBehind();
	}
}

bool UAnimalManagerComponent::FindLocationBehindTarget(float CollisionRadius, FVector& OGroundLocation)
{
	if (!Target)
		return false;

	FVector TargetLocation = Target->GetActorLocation();
	FRotator TargetRotation = Target->GetActorRotation();
	for (int Try = 0; Try < 10; Try++)
	{
		// プレイヤーの後ろ側(150度 ~ 210度)、400 ~ 800cm (来た道なので置ける可能性が高い)
		float Angle = FMath::DegreesToRadians(RandomStream.FRandRange(150.f, 210.f));
		float Distance = RandomStream.FRandRange(400.f, 800.f);
		FVector Offset(FMath::Cos(Angle) * Distance, FMath::Sin(Angle) * Distance, 0.f);
		if (CheckLocation(CollisionRadius, TargetLocation + TargetRotation.RotateVector(Offset), OGroundLocation))
			return true;
	}
	return false;
}

bool UAnimalManagerComponent::FindLocationNearTarget(float CollisionRadius, FVector& OGroundLocation)
{
	// 置ける位置がないとずっと探し続けてゲームが止まるので、回数を制限する
	for (int Try = 0; Try < 50; Try++)
	{
		FVector NewLocation = GetRandomLocationNearPlayer();
		if (CheckLocation(CollisionRadius, NewLocation, OGroundLocation))
			return true;
	}
	return FindLocationBehindTarget(CollisionRadius, OGroundLocation);
}

bool UAnimalManagerComponent::IsWildAnimal(int AnimalID) const
{
	UAnimalDataAsset* const * Data = AnimalDataMap.Find(AnimalID);
	if (!Data)
		return false;

	return (*Data)->AnimalType == EAnimalType::Wild;
}

FName UAnimalManagerComponent::GetAnimalName(int AnimalID) const
{
	UAnimalDataAsset* const* Data = AnimalDataMap.Find(AnimalID);
	if (!Data)
		return FName();
	return (*Data)->Name;
}

void UAnimalManagerComponent::SetTarget()
{
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		Target = Character;
	}
}

TMap<int32, int32> UAnimalManagerComponent::GetOwnedAnimals() const
{
	UMyGameInstance* GameInstance = GetWorld()->GetGameInstance<UMyGameInstance>();
	if (GameInstance)
	{
		return GameInstance->GetOwnedAnimals();
	}
	return TMap<int32, int32>();
}

TArray<FPrimaryAssetId> UAnimalManagerComponent::GetAllAnimalID() const
{
	TArray<FPrimaryAssetId> AnimalIDs;
	UAssetManager::Get().GetPrimaryAssetIdList(FPrimaryAssetType("AnimalData"), AnimalIDs);
	return AnimalIDs;
}

void UAnimalManagerComponent::OnAnimalLoaded(FPrimaryAssetId LoadedId)
{
	UAssetManager& AM = UAssetManager::Get();
	UObject* LoadedObj = AM.GetPrimaryAssetObject(LoadedId);
	if (UAnimalDataAsset* Data = Cast<UAnimalDataAsset>(LoadedObj))
	{
		AnimalDataMap.Add(Data->AnimalID, Data);
	}
	LoadedAnimalCount--;

	if (LoadedAnimalCount == 0)
	{
		OnLoadAnimalCompleted();
	}
}

void UAnimalManagerComponent::OnLoadAnimalCompleted()
{
	TMap<int32, int32> Owned = GetOwnedAnimals();
	if (Owned.Num() <= 0)
		return;

	for (const TPair<int32, int32>& Pair : Owned)
	{
		if (AnimalDataMap.Contains(Pair.Key))
		{
			UAnimalDataAsset* Data = AnimalDataMap[Pair.Key];
			if (Data->AnimalType != EAnimalType::Pet)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Use Wrong Animal ID or Type"));
				continue;
			}

			for (int Indedx = 0; Indedx < Pair.Value; Indedx++)
			{
				if(PetSample)
					ArrangeAroundTarget(PetSample, Data);
			}
		}
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
		if (!FindLocationNearTarget(CapsuleRadius, GroundLocation))
		{
			UE_LOG(LogTemp, Warning, TEXT("Did not find location to place animal"));
			continue;
		}

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

void UAnimalManagerComponent::ArrangeAroundTarget(TSubclassOf<AAnimal> AnimalClass, UAnimalDataAsset* Data)
{
	// DataAssetからカプセルの高さの半分をゲット
	float CapsuleHalfHeight = Data->CollisionSize.X;
	float CapsuleRadius = Data->CollisionSize.Z;
	
	// おいていける位置を探す
	FVector GroundLocation;
	if (!FindLocationNearTarget(CapsuleRadius, GroundLocation))
	{
		UE_LOG(LogTemp, Warning, TEXT("Did not find location to place animal"));
		return;
	}

	// 高さの調整
	FVector SpawnLocation = GroundLocation + FVector(0, 0, CapsuleHalfHeight);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AAnimal* Animal = GetWorld()->SpawnActor<AAnimal>(AnimalClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (Animal)
	{
		// 設定
		Animal->Init(Target, this, Data);
		FVector Offset = GroundLocation - Target->GetActorLocation();
		Animal->ChangeOffset(FVector(Offset.X, Offset.Y, 0));
		FollowingAnimals.Add(Animal);
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
