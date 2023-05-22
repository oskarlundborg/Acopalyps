// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */
/** @author Isabel Mirella Diaz Johansson */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

class AEnemyDroneBaseActor;
class UCapsuleComponent;
class ACombatManager;
class AEnemyAICharacter;
UCLASS()
class ACOPALYPS_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemyAICharacter> DefaultEnemyClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemyDroneBaseActor> DroneEnemyClass;

	AEnemyAICharacter* Spawn();

	AEnemyDroneBaseActor* SpawnDrone();

	bool IsVisibleToPlayer();

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* CapsuleComponent;

private:
	
	//ACombatManager* CombatManager;

};
