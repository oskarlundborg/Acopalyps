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
	//TODO Remove the following call before merging
	StartCombatMode();
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatManager::StartCombatMode()
{
	GetWorldTimerManager().SetTimer(RecurringSpawnCheckTimerHandle, this, &ACombatManager::RunSpawnWave, 1.f, true);
	StartOfCombat();
}


void ACombatManager::RunSpawnWave()
{
	if(WavesQueue.IsEmpty())
	{
		GetWorldTimerManager().ClearTimer(RecurringSpawnCheckTimerHandle);
		return;
	}
	if(ActiveEnemiesCount > WavesQueue.Peek()->CurrentEnemyCountMaxForNextWave) return;
	FCombatWave CurrentWave;
	WavesQueue.Dequeue(CurrentWave);
	ActiveEnemiesCount += CurrentWave.NumberOfBasicEnemies;
	for(ASpawnZone* SpawnZone : SpawnZones)
	{
		if(CurrentWave.SpawnZoneID == SpawnZone->SpawnZoneID)
		{
			SpawnZone->HandleWave(CurrentWave.NumberOfBasicEnemies);
		}
	}
	
}


void ACombatManager::GatherOverlappingActors()
{
	// populate array with all overlapping zones
	TArray<AActor*> OverlappingSpawnZones;
	ManagementZone->GetOverlappingActors(OverlappingSpawnZones, ASpawnZone::StaticClass());
	for(AActor* SpawnZone : OverlappingSpawnZones)
	{
		ASpawnZone* Zone = Cast<ASpawnZone>(SpawnZone);
		if(Zone)
		{
			SpawnZones.Add(Zone);
			Zone->CombatManager = this;
		}
	}
	// populate array with all overlapping enemies
	TArray<AActor*> OverlappingEnemies;
	ManagementZone->GetOverlappingActors(OverlappingEnemies, AEnemyAICharacter::StaticClass());
	for(AActor* BasicEnemy : OverlappingEnemies)
	{
		AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(BasicEnemy);
		if(Enemy)
		{
			ensure(Enemy != nullptr);
			AddEnemy(Enemy);
			ActiveEnemiesCount++;
		}
	}
}

void ACombatManager::AddEnemy(AEnemyAICharacter* Enemy)
{
	ManagedEnemies.Add(Enemy);
	Enemy->Manager = this;
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
