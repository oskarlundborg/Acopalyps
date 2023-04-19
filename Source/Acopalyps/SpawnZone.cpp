// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnZone.h"
#include "EnemyAICharacter.h"
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
	
}

// Called every frame
void ASpawnZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<AEnemyAICharacter*>& ASpawnZone::GetAllEnemiesFromSpawnZone()
{
	return SpawnZoneBasicEnemies;
}

