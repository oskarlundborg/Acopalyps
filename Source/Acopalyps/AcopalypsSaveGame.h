// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AcopalypsSaveGame.generated.h"

USTRUCT()
struct FActorInstance
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category=ActorInfo)
	AActor* Object;
	UPROPERTY(EditDefaultsOnly, Category=ActorInfo)
	FTransform Transform;
	UPROPERTY(EditDefaultsOnly, Category=ActorInfo)
	FVector Velocity;

	// Variables not needed on all Actors
	TOptional<FRotator> ControllerRotation = TOptional<FRotator>();
	TOptional<bool> bIsDead = TOptional<bool>();
	TOptional<float> Health = TOptional<float>();
	TOptional<int32> GunMag = TOptional<int32>();
	TOptional<class ACombatManager*> Manager = TOptional<ACombatManager*>();
};

/**
 * 
 */
UCLASS()
class ACOPALYPS_API UAcopalypsSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UAcopalypsSaveGame();

	UFUNCTION()
	void SaveGameInstance(TArray<AActor*> Actors);
	UFUNCTION()
	void LoadGameInstance();
	
	UPROPERTY(VisibleAnywhere)
	FString SlotName;
	UPROPERTY(VisibleAnywhere)
	uint32 UserIndex;

	// World Info
	FName WorldName;
	
	// Player Info
	UPROPERTY(EditDefaultsOnly, Category=Player)
	FActorInstance PlayerInstance;
	UPROPERTY(VisibleAnywhere)
	bool bPlayerSaved;

	// Actors In World Info
	UPROPERTY(EditDefaultsOnly, Category=Actors)
	TArray<FActorInstance> ActorsInWorld;
};
