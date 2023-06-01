// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */
/** @author Isabel Mirella Diaz Johansson */

#include "SpawnPoint.h"

#include "EnemyAICharacter.h"
#include "EnemyDroneBaseActor.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	RootComponent = CapsuleComponent;
}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/** Spawns walking character */
AEnemyAICharacter* ASpawnPoint::Spawn()
{
	AEnemyAICharacter* EnemyAICharacter = nullptr;
	if(IsValid(DefaultEnemyClass))
	{
		EnemyAICharacter = GetWorld()->SpawnActor<AEnemyAICharacter>(DefaultEnemyClass, GetActorLocation(), GetActorRotation());
	}
	return EnemyAICharacter;
}

/** Spawns drone */
AEnemyDroneBaseActor* ASpawnPoint::SpawnDrone()
{
	AEnemyDroneBaseActor* Drone = nullptr;
	if(IsValid(DroneEnemyClass))
	{
		Drone = GetWorld()->SpawnActor<AEnemyDroneBaseActor>(DroneEnemyClass, GetActorLocation(), GetActorRotation());
	}
	return Drone;
}

/** Checks if player can see spawn point location */
bool ASpawnPoint::IsVisibleToPlayer()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if(PlayerController && PlayerController->LineOfSightTo(this) && PlayerController->GetCharacter() != nullptr)
	{
		return PlayerController->GetCharacter()->GetActorForwardVector().Dot(GetActorLocation() - PlayerController->GetCharacter()->GetActorLocation()) > 0;
	}
	return false;
}
