// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */
/** @author Isabel Mirella Diaz Johansson */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
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

private:

	void RunSpawnWave();

	FTimerHandle RecurringSpawnCheckTimerHandle;

	UPROPERTY(VisibleAnywhere)
	TArray<AEnemyAICharacter*> ManagedEnemies = TArray<AEnemyAICharacter*>();

	UPROPERTY(VisibleAnywhere)
	TArray<AEnemyDroneBaseActor*> ManagedDrones = TArray<AEnemyDroneBaseActor*>();
	
	UPROPERTY(VisibleAnywhere)
	TArray<ASpawnZone*> SpawnZones = TArray<ASpawnZone*>();

	UPROPERTY(VisibleAnywhere)
	TArray<ACombatTrigger*> CombatTriggers = TArray<ACombatTrigger*>();

	UPROPERTY(EditAnywhere)
	TArray<FCombatWave> CombatWaves = TArray<FCombatWave>();

	TQueue<FCombatWave> WavesQueue = TQueue<FCombatWave>();

	UPROPERTY(EditAnywhere)
	UBoxComponent* ManagementZone;
	
	int ActiveEnemiesCount = 0;
};
