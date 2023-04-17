// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatManager.h"

#include "AIController.h"
#include "EnemyAICharacter.h"
#include "SpawnZone.h"
#include "Components/BoxComponent.h"

// Sets default values
ACombatManager::ACombatManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ManagementZone = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
	RootComponent = ManagementZone;
}

// Called when the game starts or when spawned
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	

	// populate array with all overlapping zones
	TArray<AActor*> OverlappingSpawnZones;
	ManagementZone->GetOverlappingActors(OverlappingSpawnZones, ASpawnZone::StaticClass());
	for(AActor* SpawnZone : OverlappingSpawnZones)
	{
		ASpawnZone* Zone = Cast<ASpawnZone>(SpawnZone);
		if(Zone) SpawnZones.Add(Zone);
	}
	// populate array with all overlapping enemies
	TArray<AActor*> OverlappingEnemies;
	ManagementZone->GetOverlappingActors(OverlappingEnemies, AEnemyAICharacter::StaticClass());
	for(AActor* BasicEnemy : OverlappingEnemies)
	{
		AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(BasicEnemy);
		if(Enemy)
		{
			ManagementZoneBasicEnemies.Add(Enemy);
			Enemy->Manager = this;
			ActiveEnemiesCount++;
		}
	}

	for (FCombatWave Wave : CombatWaves)
	{
		WavesQueue.Enqueue(Wave);
	}
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatManager::RemoveEnemy(AEnemyAICharacter* EnemyToRemove)
{
	ManagementZoneBasicEnemies.Remove(EnemyToRemove);
	ActiveEnemiesCount--;
}

void ACombatManager::RunSpawnWave()
{
	FCombatWave CurrentWave;
	WavesQueue.Dequeue(CurrentWave);
	//GetWorld()->SpawnActor()
}
