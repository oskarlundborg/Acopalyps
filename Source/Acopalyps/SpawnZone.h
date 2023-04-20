// Fill out your copyright notice in the Description page of Project Settings.

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

	ACombatManager* CombatManager = nullptr;

	void HandleWave(int EnemiesToSpawn);

	UPROPERTY(EditAnywhere)
	int SpawnZoneID;
	
private:

	void GatherOverlappingSpawnPoints();

	void Spawn();

	FTimerHandle SpawnTimerHandle;

	UPROPERTY(EditAnywhere)
	TArray<ASpawnPoint*> SpawnPoints = TArray<ASpawnPoint*>();

	UPROPERTY(EditAnywhere)
	UBoxComponent* SpawnZoneZone;
	
	UPROPERTY(EditAnywhere, Category="Spawning")
	int SpawnDelay;

	int NumberOfEnemiesLeftToSpawn = 0;
};
