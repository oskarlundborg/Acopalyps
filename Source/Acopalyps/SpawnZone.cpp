// Fill out your copyright notice in the Description page of Project Settings.


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


void ASpawnZone::HandleWave(int EnemiesToSpawn)
{
	NumberOfEnemiesLeftToSpawn += EnemiesToSpawn;
	if(!GetWorldTimerManager().TimerExists(SpawnTimerHandle))
		Spawn();
}

void ASpawnZone::Spawn()
{
	for(ASpawnPoint* SpawnPoint : SpawnPoints)
	{
		if(NumberOfEnemiesLeftToSpawn > 0 && !SpawnPoint->IsVisibleToPlayer())
		{
			AEnemyAICharacter* SpawnedEnemy = SpawnPoint->Spawn();
			if(SpawnedEnemy)
			{
				CombatManager->AddEnemy(SpawnedEnemy);
				NumberOfEnemiesLeftToSpawn--;
			}
		}
	}
	if(NumberOfEnemiesLeftToSpawn > 0)
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawnZone::Spawn, 2.f, false);
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
