// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */
/** @author Isabel Mirella Diaz Johansson */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnZone.generated.h"

class ASpawnPoint;
class ACombatManager;
class UBoxComponent;
class AEnemyAICharacter;
UCLASS()
class ACOPALYPS_API ASpawnZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Pointer to combat manager instance*/
	UPROPERTY(SaveGame)
	ACombatManager* CombatManager = nullptr;

	/** Spawns enemies with delays specified in wave struct and recieved in parameters
	 * @param BasicEnemiesToSpawn number of enemies to spawn
	 * @param DronesToSpawn number of drones to spawn
	 * @param Delay time to wait in between spawnes
	 */
	void HandleWave(int BasicEnemiesToSpawn, int DronesToSpawn, double Delay);

	/** Unique id for instance that is placed in world */
	UPROPERTY(EditAnywhere, SaveGame)
	int SpawnZoneID;
	
private:

	/** Runs to populate collections with pointers to spawn points Spawn zone - zone component is overlapping */
	void GatherOverlappingSpawnPoints();

	/** Loops array of points and spawnes specified enemies with specified delays inbetween spawnes*/
	void Spawn();

	/** Timer handle to handle spawn delays*/
	FTimerHandle SpawnTimerHandle;

	/** Array of spawn points that spawn zone overlaps*/
	UPROPERTY(EditAnywhere, SaveGame)
	TArray<ASpawnPoint*> SpawnPoints = TArray<ASpawnPoint*>();

	/** Zone that overlaps area in world that spawn zone instance considers a spawn area */
	UPROPERTY(EditAnywhere, SaveGame)
	UBoxComponent* SpawnZoneZone;

	/** Delay until first spawn starts*/
	UPROPERTY(EditAnywhere, Category="Spawning", SaveGame)
	int SpawnDelay;

	/** Self explanatory*/
	int NumberOfBasicEnemiesLeftToSpawn = 0;
	/** Self explanatory*/
	int NumberOfDronesLeftToSpawn = 0;
};
