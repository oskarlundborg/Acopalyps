// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "CombatManager.generated.h"

class UBoxComponent;
class AAIController;
class AEnemyAICharacter;
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
	int CurrentEnemyCountMaxForNextSpawn;

	//TODO: Tillgång till spawnZone, för spawnpoints, timer för spawn, decrement numBasicEnemy, 
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

private:

	UFUNCTION(BlueprintCallable, Category= "Enemy Spawn Wave", meta=(AllowPrivateAccess = true))
	void RunSpawnWave();

	UPROPERTY(VisibleAnywhere)
	TArray<AEnemyAICharacter*> ManagementZoneBasicEnemies;
	
	UPROPERTY(VisibleAnywhere)
	TArray<ASpawnZone*> SpawnZones;

	UPROPERTY(EditAnywhere)
	TArray<FCombatWave> CombatWaves;

	TQueue<FCombatWave> WavesQueue;

	UPROPERTY(EditAnywhere)
	UBoxComponent* ManagementZone;

	int ActiveEnemiesCount = 0;
};
