// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson
	@author Jonathan Rubensson
*/


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentHealth = MaxHealth;
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UHealthComponent::GetHealth() const
{
	return CurrentHealth;
}

float UHealthComponent::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}

void UHealthComponent::SetHealth(float NewHealth)
{
	CurrentHealth = FMath::Min(MaxHealth, FMath::Max(0, NewHealth));
}

void UHealthComponent::SetMaxHealth(float NewHealth)
{
	MaxHealth = NewHealth;
}

void UHealthComponent::RefillHealth()
{
	CurrentHealth = MaxHealth;
}

bool UHealthComponent::IsDead() const
{
	return CurrentHealth <= 0;
}
