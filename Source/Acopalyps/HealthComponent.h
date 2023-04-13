// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACOPALYPS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsDead() const;
	
	float GetHealth() const;
	void SetHealth(float NewHealth);
	float GetHealthPercent() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Health, meta=(AllowPrivateAccess = true))
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Health, meta=(AllowPrivateAccess = true))
	float MaxHealth;

		
};
