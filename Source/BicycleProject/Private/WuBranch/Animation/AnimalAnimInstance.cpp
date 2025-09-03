// Fill out your copyright notice in the Description page of Project Settings.


#include "WuBranch/Animation/AnimalAnimInstance.h"
#include "WuBranch/Actor/Animal.h"

UAnimalAnimInstance::UAnimalAnimInstance()
	: Animal(nullptr)
{
}

void UAnimalAnimInstance::UpdateAnimation(float DeltaTime)
{
	if (!Animal)
		SetAnimalInstance();

	if (Animal)
	{
		Speed = Animal->GetCurrentSpeed();
	}
}

void UAnimalAnimInstance::SetAnimalInstance()
{
	if (AAnimal* Actor = Cast<AAnimal>(TryGetPawnOwner()))
	{
		Animal = Actor;
	}
}
