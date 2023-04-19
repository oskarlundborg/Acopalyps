// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"

#include "CombatManager.h"
#include "EnemyAICharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

AEnemyAICharacter* ASpawnPoint::Spawn(TSubclassOf<AEnemyAICharacter> EnemyClass)
{
	AEnemyAICharacter* EnemyAICharacter = nullptr;
	if(IsValid(EnemyClass))
	{
		EnemyAICharacter = GetWorld()->SpawnActor<AEnemyAICharacter>(EnemyClass, GetActorLocation(), GetActorRotation());
	}
	return EnemyAICharacter;
}

bool ASpawnPoint::IsVisibleToPlayer()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if(PlayerController && PlayerController->LineOfSightTo(this))
	{
		return PlayerController->GetCharacter()->GetActorForwardVector().Dot(GetActorLocation() - PlayerController->GetCharacter()->GetActorLocation()) > 0;
	}
	return false;
}
