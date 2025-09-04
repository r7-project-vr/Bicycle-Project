// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimalManagerComponent.generated.h"

class AAnimal;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BICYCLEPROJECT_API UAnimalManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnimalManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// ついてくる動物から対象動物を消す
	/// </summary>
	/// <param name="Animal"></param>
	void ReduceAnimal(AAnimal* Animal);

private:

	/// <summary>
	/// プレイヤーを設定
	/// </summary>
	void SetTarget();

	/// <summary>
	/// 動物を対象の周りに配置
	/// </summary>
	void ArrangeAroundTarget(TArray<TSubclassOf<AAnimal>> Animals);

	/// <summary>
	/// 対象の周りの位置をランダムで一つをとる
	/// </summary>
	/// <returns>位置</returns>
	FVector GetRandomLocationNearPlayer();

	/// <summary>
	/// 位置の確認、何かとぶつかるか
	/// </summary>
	/// <param name="Location">位置</param>
	/// <param name="OGroundLocation">地面の位置</param>
	/// <returns>true: 大丈夫, false: 使えない</returns>
	bool CheckLocation(FVector Location, FVector& OGroundLocation);

	/// <summary>
	/// ついていく対象
	/// </summary>
	TObjectPtr<ACharacter> Target;

	/// <summary>
	/// ついてくる動物
	/// </summary>
	TArray<AAnimal*> FollowingAnimals;

	/// <summary>
	/// 対象と動物の最大距離
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Radius;

	/// <summary>
	/// ランダム生成器
	/// </summary>
	FRandomStream RandomStream;
};
