// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */
/** @author Isabel Mirella Diaz Johansson */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "Templates/Tuple.h"
#include "CombatManager.generated.h"

class ACombatTrigger;
class UBoxComponent;
class AAIController;
class AEnemyAICharacter;
class AEnemyDroneBaseActor;
class UBehaviorTree;
class ASpawnZone;

/** Struct for storing all relevant information regarding spawning of enemies */
USTRUCT(BlueprintType, Category = "Enemy Spawn Wave")
struct FCombatWave
{
	GENERATED_BODY()

	/** Id for the spawn zone in use*/
	UPROPERTY(EditAnywhere, Category = "Enemy Spawn Wave")
	int SpawnZoneID;

	/** Number of walking enemies to spawn from spawnzone*/
	UPROPERTY(EditAnywhere, Category = "Enemy Spawn Wave")
	int NumberOfBasicEnemies;

	/** Number of drones to spawn from spawnzone*/
	UPROPERTY(EditAnywhere, Category = "Enemy Spawn Wave")
	int NumberOfDrones;

	/** Number of enemies that are alive when spawning next wave*/
	UPROPERTY(EditAnywhere, Category = "Enemy Spawn Wave")
	int CurrentEnemyCountMaxForNextWave;

	/** Seconds to wait when spawning criteria is met, to start the combat wave and spawning process*/
	UPROPERTY(EditAnywhere, Category = "Enemy Spawn Wave")
	double DelayInSecondsForWaveAfterCriteriaReached;

	// ========== Sound variables ======== //
	UPROPERTY(EditAnywhere, Category= "Sound")
	USoundWave* StartSoundWave;

	UPROPERTY(EditAnywhere, Category= "Sound")
	float VolumeMultiplyer;

	UPROPERTY(EditAnywhere, Category= "Sound")
	float PitchMultiplyer;
};

UCLASS()
class ACOPALYPS_API ACombatManager : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ACombatManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Removes pointer to enemy from managers collection, and if queue is empty and no active enemies left calls to end combat
	 * @param EnemyToRemove pointer to enemy to remove from collection
	 */
	void RemoveEnemy(AEnemyAICharacter* EnemyToRemove);

	/** Removes pointer to drone from managers collection, and if queue is empty and no active enemies left calls to end combat
	 * @param DroneToRemove pointer to drone to remove from collection
	 */
	void RemoveDrone(AEnemyDroneBaseActor* DroneToRemove);

	/** Event for when combat is started*/
	UFUNCTION(BlueprintImplementableEvent)
	void StartOfCombat();

	/** Event for when combat has ended*/
	UFUNCTION(BlueprintImplementableEvent)
	void EndOfCombat();

	/** Runs to populate collections with pointers to enemies, spawnzones and combattriggers that Management Zone component is overlapping */
	void GatherOverlappingActors();

	/** Adds enemy pointer that is placed or is spawned within combat bounds to collection
	 * @param Enemy pointer to enemy character instance 
	 */
	void AddEnemy(AEnemyAICharacter* Enemy);

	/** Adds drone pointer that is spawned within combat bounds to collection
	 * @param Drone pointer to enemy character instance
	 */
	void AddDrone(AEnemyDroneBaseActor* Drone);

	/** At combat start method runs to start enemy combat behaviour, and triggers potential triggers for combat related functions*/
	UFUNCTION(BlueprintCallable, Category= "Enemy Spawn Wave", meta=(AllowPrivateAccess = true))
	void StartCombatMode();

	/** Number of current active enemies, aka enemies that are alive within combat management zone*/
	UPROPERTY(VisibleAnywhere, SaveGame)
	int ActiveEnemiesCount = 0;

	/** Is true when combat has started */
	UPROPERTY(VisibleAnywhere)
	bool bCombatStarted = false;

	/** Resets all collections and information about progress in a combat, to enable starting combat from beginning again*/
	void ResetCombat();

private:

	/** Initiates next combat wave in queue if queue not empty and combat requirements are met */
	void RunSpawnWave();
	
	UPROPERTY(SaveGame)
	FTimerHandle RecurringSpawnCheckTimerHandle;

	/** Array of enemy character pointers that are alive within combat management zone*/
	UPROPERTY(VisibleAnywhere, SaveGame)
	TArray<AEnemyAICharacter*> ManagedEnemies = TArray<AEnemyAICharacter*>();

	/** Array of drones pointers that are alive within combat management zone*/
	UPROPERTY(VisibleAnywhere, SaveGame)
	TArray<AEnemyDroneBaseActor*> ManagedDrones = TArray<AEnemyDroneBaseActor*>();

	/** Array of spawn zone pointers within combat management zone*/
	UPROPERTY(VisibleAnywhere, SaveGame)
	TArray<ASpawnZone*> SpawnZones = TArray<ASpawnZone*>();

	/** Array of combat trigger pointers within combat management zone*/
	UPROPERTY(VisibleAnywhere, SaveGame)
	TArray<ACombatTrigger*> CombatTriggers = TArray<ACombatTrigger*>();

	/** Array of combat waves that this instance of combat manager should start during its life time*/
	UPROPERTY(EditAnywhere, SaveGame)
	TArray<FCombatWave> CombatWaves = TArray<FCombatWave>();

	/** Queue of waves that this instance of combat manager should initiate during its life time*/
	TQueue<FCombatWave> WavesQueue = TQueue<FCombatWave>();

	/** Zone that overlaps area in world that manager instance considers a combat area */
	UPROPERTY(EditAnywhere, SaveGame)
	UBoxComponent* ManagementZone;
};
