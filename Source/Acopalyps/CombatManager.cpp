// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatManager.h"

#include "AIController.h"
#include "CombatTrigger.h"
#include "EnemyAICharacter.h"
#include "EnemyDroneBaseActor.h"
#include "SpawnZone.h"
#include "AI/NavigationSystemBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

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
	//StartCombatMode();
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatManager::StartCombatMode()
{
	if(GetWorldTimerManager().TimerExists(RecurringSpawnCheckTimerHandle)) return;
	for(AEnemyAICharacter* Enemy : ManagedEnemies)
	{
		Enemy->InitializeController();
	}
	GetWorldTimerManager().SetTimer(RecurringSpawnCheckTimerHandle, this, &ACombatManager::RunSpawnWave, 1.f, true);
	for(ACombatTrigger* Trigger : CombatTriggers)
	{
		Trigger->TriggerBox->Deactivate();
	}
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
	ActiveEnemiesCount += CurrentWave.NumberOfBasicEnemies + CurrentWave.NumberOfDrones;
	for(ASpawnZone* SpawnZone : SpawnZones)
	{
		if(CurrentWave.SpawnZoneID == SpawnZone->SpawnZoneID)
		{
			if (CurrentWave.StartSoundWave)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), CurrentWave.StartSoundWave, CurrentWave.VolumeMultiplyer, CurrentWave.PitchMultiplyer, 0.1f);
			}	
			SpawnZone->HandleWave(CurrentWave.NumberOfBasicEnemies, CurrentWave.NumberOfDrones, CurrentWave.DelayInSecondsForWaveAfterCriteriaReached);
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
	// populate array with all overlapping combat triggers
	TArray<AActor*> OverlappingCombatTriggers;
	ManagementZone->GetOverlappingActors(OverlappingCombatTriggers, ACombatTrigger::StaticClass());
	for(AActor* CombatTrigger : OverlappingCombatTriggers)
	{
		ACombatTrigger* Trigger = Cast<ACombatTrigger>(CombatTrigger);
		if(Trigger)
		{
			ensure(Trigger != nullptr);
			CombatTriggers.Add(Trigger);
			Trigger->Manager = this;
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

void ACombatManager::AddDrone(AEnemyDroneBaseActor* Drone)
{
	ManagedDrones.Add(Drone);
	Drone->CombatManager = this;
}

void ACombatManager::RemoveDrone(AEnemyDroneBaseActor* DroneToRemove)
{
	ManagedDrones.Remove(DroneToRemove);
	ActiveEnemiesCount--;
	if(WavesQueue.IsEmpty() && ActiveEnemiesCount == 0)
	{
		EndOfCombat();
	}
}
