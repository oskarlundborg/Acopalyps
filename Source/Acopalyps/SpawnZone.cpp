// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */
/** @author Isabel Mirella Diaz Johansson */

#include "SpawnZone.h"

#include "CombatManager.h"
#include "EnemyAICharacter.h"
#include "EnemyAIController.h"
#include "SpawnPoint.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnZone::ASpawnZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnZoneZone = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
	RootComponent = SpawnZoneZone;
}

// Called when the game starts or when spawned
void ASpawnZone::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle GatherSpawnPointHandler;
	GetWorldTimerManager().SetTimer(GatherSpawnPointHandler, this, &ASpawnZone::GatherOverlappingSpawnPoints, 0.1f, false);
}

// Called every frame
void ASpawnZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASpawnZone::HandleWave(int BasicEnemiesToSpawn, int DronesToSpawn, double Delay)
{
	NumberOfBasicEnemiesLeftToSpawn += BasicEnemiesToSpawn;
	NumberOfDronesLeftToSpawn += DronesToSpawn;
	if(!GetWorldTimerManager().TimerExists(SpawnTimerHandle))
	{
		if(Delay == 0)
		{
			Spawn();
		}
		else
		{
			GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawnZone::Spawn, Delay, false);
		}
	}
	else
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawnZone::Spawn, Delay, false);
	}
		
}

void ASpawnZone::Spawn()
{
	for(ASpawnPoint* SpawnPoint : SpawnPoints)
	{
		if(NumberOfBasicEnemiesLeftToSpawn > 0 && !SpawnPoint->IsVisibleToPlayer())
		{
			AEnemyAICharacter* SpawnedEnemy = SpawnPoint->Spawn();
			if(SpawnedEnemy)
			{
				SpawnedEnemy->InitializeController();
				CombatManager->AddEnemy(SpawnedEnemy);
				NumberOfBasicEnemiesLeftToSpawn--;
			}
		} else if(NumberOfDronesLeftToSpawn > 0 && !SpawnPoint->IsVisibleToPlayer())
		{
			AEnemyDroneBaseActor* SpawnedDrone = SpawnPoint->SpawnDrone();
			if(SpawnedDrone)
			{
				CombatManager->AddDrone(SpawnedDrone);
				NumberOfDronesLeftToSpawn--;
			}
		}
	}
	if(NumberOfBasicEnemiesLeftToSpawn + NumberOfDronesLeftToSpawn > 0)
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawnZone::Spawn, 1.f, false);
	} else
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}
}


void ASpawnZone::GatherOverlappingSpawnPoints()
{
	// populate array with all overlapping spawn points
	TArray<AActor*> OverlappingSpawnPoints;
	SpawnZoneZone->GetOverlappingActors(OverlappingSpawnPoints, ASpawnPoint::StaticClass());
	for(AActor* SpawnPoint : OverlappingSpawnPoints)
	{
		ASpawnPoint* Point = Cast<ASpawnPoint>(SpawnPoint);
		if(ensure(Point != nullptr)) SpawnPoints.Add(Point);
	}
}
