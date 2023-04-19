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
	ManagementZone->SetCollisionProfileName("CombatManager");
	RootComponent = ManagementZone;
}

// Called when the game starts or when spawned
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle GatherOverlapHandle;
	GetWorldTimerManager().SetTimer(GatherOverlapHandle, this, &ACombatManager::GatherOverlappingActors, 0.1, false);
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

void ACombatManager::RunSpawnWave()
{
	FCombatWave CurrentWave;
	WavesQueue.Dequeue(CurrentWave);
	//GetWorld()->SpawnActor()
}



void ACombatManager::GatherOverlappingActors()
{
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
	ensure(!OverlappingEnemies.IsEmpty());
	for(AActor* BasicEnemy : OverlappingEnemies)
	{
		AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(BasicEnemy);
		if(Enemy)
		{
			ensure(Enemy != nullptr);
			AddEnemy(Enemy);
			Enemy->Manager = this;
		}
	}
}

/*
void ACombatManager::SetCombatManagerToSpawnPoints()
{
	for(AActor* SpawnZone : SpawnZones)
	{
		//TODO: sätta alla spawnzones manager
	}
}
*/

void ACombatManager::AddEnemy(AEnemyAICharacter* Enemy)
{
	ManagedEnemies.Add(Enemy);
	ActiveEnemiesCount++;
}

void ACombatManager::RemoveEnemy(AEnemyAICharacter* EnemyToRemove)
{
	ManagedEnemies.Remove(EnemyToRemove);
	ActiveEnemiesCount--;
	if(WavesQueue.IsEmpty() && ActiveEnemiesCount == 0)
	{
		EndOfCombat();
	}
}
