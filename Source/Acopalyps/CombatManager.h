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

USTRUCT(BlueprintType, Category = "Enemy Spawn Wave")
struct FCombatWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Enemy Spawn Wave")
	int SpawnZoneID;
	
	UPROPERTY(EditAnywhere, Category = "Enemy Spawn Wave")
	int NumberOfBasicEnemies;

	UPROPERTY(EditAnywhere, Category = "Enemy Spawn Wave")
	int NumberOfDrones;

	UPROPERTY(EditAnywhere, Category = "Enemy Spawn Wave")
	int CurrentEnemyCountMaxForNextWave;

	UPROPERTY(EditAnywhere, Category = "Enemy Spawn Wave")
	double DelayInSecondsForWaveAfterCriteriaReached;

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

	void RemoveEnemy(AEnemyAICharacter* EnemyToRemove);

	void RemoveDrone(AEnemyDroneBaseActor* DroneToRemove);

	UFUNCTION(BlueprintImplementableEvent)
	void StartOfCombat();

	UFUNCTION(BlueprintImplementableEvent)
	void EndOfCombat();

	void GatherOverlappingActors();

	void AddEnemy(AEnemyAICharacter* Enemy);

	void AddDrone(AEnemyDroneBaseActor* Drone);

	UFUNCTION(BlueprintCallable, Category= "Enemy Spawn Wave", meta=(AllowPrivateAccess = true))
	void StartCombatMode();

	UPROPERTY(VisibleAnywhere, SaveGame)
	int ActiveEnemiesCount = 0;

	UPROPERTY(VisibleAnywhere)
	bool bCombatStarted = false;

	void ResetCombat();

private:

	void RunSpawnWave();

	UPROPERTY(SaveGame)
	FTimerHandle RecurringSpawnCheckTimerHandle;

	UPROPERTY(VisibleAnywhere, SaveGame)
	TArray<AEnemyAICharacter*> ManagedEnemies = TArray<AEnemyAICharacter*>();

	UPROPERTY(VisibleAnywhere, SaveGame)
	TArray<AEnemyDroneBaseActor*> ManagedDrones = TArray<AEnemyDroneBaseActor*>();
	
	UPROPERTY(VisibleAnywhere, SaveGame)
	TArray<ASpawnZone*> SpawnZones = TArray<ASpawnZone*>();

	UPROPERTY(VisibleAnywhere, SaveGame)
	TArray<ACombatTrigger*> CombatTriggers = TArray<ACombatTrigger*>();

	UPROPERTY(EditAnywhere, SaveGame)
	TArray<FCombatWave> CombatWaves = TArray<FCombatWave>();

	TQueue<FCombatWave> WavesQueue = TQueue<FCombatWave>();

	UPROPERTY(EditAnywhere, SaveGame)
	UBoxComponent* ManagementZone;
};
