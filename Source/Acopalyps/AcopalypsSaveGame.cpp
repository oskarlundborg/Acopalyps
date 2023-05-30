// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"

#include "LevelSpawner.h"
#include "AcopalypsCharacter.h"
#include "CombatManager.h"
#include "EnemyAICharacter.h"
#include "LevelStreamerSubsystem.h"
#include "GameFramework/PawnMovementComponent.h"

class ULevelStreamerSubsystem;

void UAcopalypsSaveGame::SaveGameInstance(const AAcopalypsCharacter* Player, TArray<AActor*> Actors)
{
	Timestamp = FDateTime::Now();
	
	PlayerInstance = {
		.Class		= Player->GetClass(),
		.Transform  = Player->GetTransform(),
		.Rotation	= Player->GetController()->GetControlRotation(),
		.Velocity	= Player->GetVelocity(),
		.bIsDead	= Player->HealthComponent->IsDead(),
		.Health		= Player->HealthComponent->GetHealth(),
		.GunMag		= Player->Gun->CurrentMag,
	};
	
	SubLevels = Player->LoadedLevels;

	GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Green, TEXT("Game Saved!"));
}

void UAcopalypsSaveGame::LoadGameInstance(AAcopalypsCharacter* Player, TArray<AActor*>& Actors)
{
	// Set for deletion.
	for( AActor* Actor : Actors )
	{
		if( ClassFilter.Contains(Actor->GetClass()) )
		{
			if( AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(Actor))
			{
				Enemy->DetachFromControllerPendingDestroy();
			}
			Actor->Destroy();
		}
	}
	
	ULevelStreamerSubsystem* LevelStreamSubsystem = Player->GetWorld()->GetGameInstance()->GetSubsystem<ULevelStreamerSubsystem>();
	TArray<int> LevelKeys;
	// Unload all loaded levels
	LevelStreamSubsystem->LevelMap.GetKeys(LevelKeys);
	for( int i = 0; i < LevelKeys.Num(); i++ )
	{
		LevelStreamSubsystem->UnloadLevel(LevelKeys[i]);
	}
	// Load all levels from save
	for( int i = 0; i < SubLevels.Num(); i++ )
	{
		UE_LOG(LogTemp, Display, TEXT("%i"), SubLevels[i].ID )
		LevelStreamSubsystem->LoadLevel(SubLevels[i]);
	}
	
	// Set player data.
	Player->SetActorTransform(PlayerInstance.Transform);
	Player->GetController()->SetControlRotation(PlayerInstance.Rotation);
	Player->HealthComponent->SetHealth(PlayerInstance.Health);
	Player->Gun->CurrentMag = PlayerInstance.GunMag;
	Player->GetMovementComponent()->Velocity = PlayerInstance.Velocity;
	
	GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Cyan, TEXT("Game Loaded."));
}
