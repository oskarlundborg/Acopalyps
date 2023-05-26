// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"

#include "LevelSpawner.h"
#include "AcopalypsCharacter.h"
#include "EnemyAICharacter.h"
#include "EnemyAIController.h"
#include "EnemyDroneBaseActor.h"
#include "LevelStreamerSubsystem.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PawnMovementComponent.h"

class ULevelStreamerSubsystem;

void UAcopalypsSaveGame::SaveGameInstance(const UWorld* World, TArray<AActor*> Actors)
{
	// Save level name
	//WorldName = FName(World->GetName());
	
	// Store data from actors in scene
	for( AActor* Actor : Actors )
	{
		// Get iteration's actor class
		const auto ActorClass = Actor->GetClass();
		UE_LOG(LogTemp, Display, TEXT("Actor Class: %s"), *Actor->GetClass()->GetName())
		
		// Save player specific data
		if( ActorClass == PlayerClass )
		{
			const AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(World->GetFirstPlayerController()->GetCharacter());
			PlayerInstance = {
				.Class		= ActorClass,
				.Transform  = Player->GetTransform(),
				.Rotation	= Player->GetController()->GetControlRotation(),
				.Velocity	= Player->GetVelocity(),
				.bIsDead	= Player->HealthComponent->IsDead(),
				.Health		= Player->HealthComponent->GetHealth(),
				.GunMag		= Player->Gun->CurrentMag,
			};
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
			UE_LOG(LogTemp, Display, TEXT("## Player Saved ###########################"))
			UE_LOG(LogTemp, Display, TEXT("Class: %s"), *PlayerInstance.Class->GetName())
			UE_LOG(LogTemp, Display, TEXT("Transform: %s"), *PlayerInstance.Transform.ToString())
			UE_LOG(LogTemp, Display, TEXT("IsDead: %i"), PlayerInstance.bIsDead)
			UE_LOG(LogTemp, Display, TEXT("Health: %f"), PlayerInstance.Health)
			UE_LOG(LogTemp, Display, TEXT("GunMag: %i"), PlayerInstance.GunMag)
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
			SubLevels = Player->LoadedLevels;
		}
		// Save enemy specific data
		else if( ActorClass == EnemyClass )
		{
			const AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(Actor);
			ActorsInWorld.Add({
				.Class		= ActorClass,
				.Transform	= Enemy->GetTransform(),
				.bIsDead	= Enemy->IsDead(),
				.Health		= Enemy->HealthComponent->GetHealth(),
				.GunMag		= Enemy->Gun->CurrentMag,
			});
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
			UE_LOG(LogTemp, Display, TEXT("## Enemy Saved ############################"))
			UE_LOG(LogTemp, Display, TEXT("Class: %s"), *ActorsInWorld.Last().Class->GetName())
			UE_LOG(LogTemp, Display, TEXT("Transform: %s"), *ActorsInWorld.Last().Transform.ToString())
			UE_LOG(LogTemp, Display, TEXT("IsDead: %i"), ActorsInWorld.Last().bIsDead)
			UE_LOG(LogTemp, Display, TEXT("Health: %f"), ActorsInWorld.Last().Health)
			UE_LOG(LogTemp, Display, TEXT("GunMag: %i"), ActorsInWorld.Last().GunMag)
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
		}
		else if( ActorClass == EnemyDroneClass )
		{
			const AEnemyDroneBaseActor* Drone = Cast<AEnemyDroneBaseActor>(Actor);
			ActorsInWorld.Add({
				.Class		= ActorClass,
				.Transform	= Drone->GetTransform(),
				.bIsDead	= Drone->IsDead(),
				.Health		= Drone->HealthComponent->GetHealth(),
				.MeshComp	= Drone->DroneMesh,
			});
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
			UE_LOG(LogTemp, Display, TEXT("## Enemy Saved ############################"))
			UE_LOG(LogTemp, Display, TEXT("Class: %s"), *ActorsInWorld.Last().Class->GetName())
			UE_LOG(LogTemp, Display, TEXT("Transform: %s"), *ActorsInWorld.Last().Transform.ToString())
			UE_LOG(LogTemp, Display, TEXT("IsDead: %i"), ActorsInWorld.Last().bIsDead)
			UE_LOG(LogTemp, Display, TEXT("Health: %f"), ActorsInWorld.Last().Health)
			UE_LOG(LogTemp, Display, TEXT("GunMag: %i"), ActorsInWorld.Last().GunMag)
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
		}
		// Save non static actors data
		else if( ActorClass == StaticMeshClass && Actor->GetRootComponent() != nullptr && Actor->GetRootComponent()->IsSimulatingPhysics() && Actor->Owner == nullptr )
		{
			UE_LOG(LogTemp, Display, TEXT("Class: %s"), *ActorClass->GetName())
			const AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor);
			ActorsInWorld.Add({
				.Class		= ActorClass,
				.Transform	= StaticMeshActor->GetActorTransform(),
				.Mesh		= StaticMeshActor->GetStaticMeshComponent()->GetStaticMesh(),
			});
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
			UE_LOG(LogTemp, Display, TEXT("## Actor Saved ¤###########################"))
			UE_LOG(LogTemp, Display, TEXT("Class: %s"), *ActorsInWorld.Last().Class->GetName())
			UE_LOG(LogTemp, Display, TEXT("Transform: %s"), *ActorsInWorld.Last().Transform.ToString())
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Green, TEXT("Game Saved!"));
}

void UAcopalypsSaveGame::LoadGameInstance(UWorld* World, TArray<AActor*>& Actors)
{
	// Open the saved world if different
	//if( WorldName != FName(World->GetName()) )
	//{
	//	UGameplayStatics::OpenLevel(World, WorldName, false);
	//}
	ULevelStreamerSubsystem* LevelStreamSubsystem = World->GetGameInstance()->GetSubsystem<ULevelStreamerSubsystem>();
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
	
	// Set for deletion.
	DestroySceneActors(Actors);
	
	for (int i = 0; i < SubLevels.Num(); i++)
	{
		Actors.Last()->GetGameInstance()->GetSubsystem<ULevelStreamerSubsystem>()->LoadLevel(SubLevels[i]);
	}
	// Recreate saved state.
	for( const FActorInstance Actor : ActorsInWorld )
	{
		// If actor is enemy, set specific data
		if( Actor.Class == EnemyClass )
		{
			if( Actor.bIsDead ) continue;
			
			AEnemyAICharacter* Enemy = World->SpawnActor<AEnemyAICharacter>(
				Actor.Class,
				Actor.Transform.GetLocation(),
				Actor.Transform.Rotator()
				);
			Enemy->HealthComponent->SetHealth(Actor.Health);
			Enemy->Gun->CurrentMag = Actor.GunMag;
			Cast<AEnemyAIController>(Enemy->GetController())->Initialize();
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
			UE_LOG(LogTemp, Display, TEXT("## Enemy Loaded ###########################"))
			UE_LOG(LogTemp, Display, TEXT("Class: %s"), *Actor.Class->GetName())
			UE_LOG(LogTemp, Display, TEXT("Transform: %s"), *Actor.Transform.ToString())
			UE_LOG(LogTemp, Display, TEXT("IsDead: %i"), Actor.bIsDead)
			UE_LOG(LogTemp, Display, TEXT("Health: %f"), Actor.Health)
			UE_LOG(LogTemp, Display, TEXT("GunMag: %i"), Actor.GunMag)
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
		}
		else if( Actor.Class == EnemyDroneClass )
		{
			if( Actor.bIsDead ) continue;
			
			AEnemyDroneBaseActor* Drone = World->SpawnActor<AEnemyDroneBaseActor>(
				Actor.Class,
				Actor.Transform.GetLocation(),
				Actor.Transform.Rotator()
				);
			Drone->HealthComponent->SetHealth(Actor.Health);
			Drone->DroneMesh = Actor.MeshComp;
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
			UE_LOG(LogTemp, Display, TEXT("## Drone Loaded ###########################"))
			UE_LOG(LogTemp, Display, TEXT("Class: %s"), *Actor.Class->GetName())
			UE_LOG(LogTemp, Display, TEXT("Transform: %s"), *Actor.Transform.ToString())
			UE_LOG(LogTemp, Display, TEXT("IsDead: %i"), Actor.bIsDead)
			UE_LOG(LogTemp, Display, TEXT("Health: %f"), Actor.Health)
			UE_LOG(LogTemp, Display, TEXT("GunMag: %i"), Actor.GunMag)
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
		}
		else if( Actor.Class == StaticMeshClass ) // Handle all static mesh data in scene
		{
			AStaticMeshActor* StaticMesh = World->SpawnActor<AStaticMeshActor>(
				Actor.Class,
				Actor.Transform.GetLocation(),
				Actor.Transform.Rotator()
				);
			StaticMesh->SetMobility(EComponentMobility::Movable);
			StaticMesh->GetStaticMeshComponent()->SetStaticMesh(Actor.Mesh);
			StaticMesh->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
			StaticMesh->GetStaticMeshComponent()->SetRelativeTransform(Actor.Transform);
			StaticMesh->GetStaticMeshComponent()->SetSimulatePhysics(true);
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
			UE_LOG(LogTemp, Display, TEXT("## Static Mesh Loaded #####################"))
			UE_LOG(LogTemp, Display, TEXT("Static Mesh: %s"), *StaticMesh->GetClass()->GetName())
			UE_LOG(LogTemp, Display, TEXT("Class: %s"), *Actor.Class->GetName())
			UE_LOG(LogTemp, Display, TEXT("Transform: %s"), *Actor.Transform.ToString())
			DrawDebugBox(World, Actor.Transform.GetLocation(), FVector(10, 10, 10), FColor::Red, false, 5);
			UE_LOG(LogTemp, Display, TEXT("###########################################"))
		}
	}
	// Set player data.
	AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(World->GetFirstPlayerController()->GetCharacter());
	Player->SetActorTransform(PlayerInstance.Transform);
	Player->GetController()->SetControlRotation(PlayerInstance.Rotation);
	Player->HealthComponent->SetHealth(PlayerInstance.Health);
	Player->Gun->CurrentMag = PlayerInstance.GunMag;
	Player->GetMovementComponent()->Velocity = PlayerInstance.Velocity;
	UE_LOG(LogTemp, Display, TEXT("###########################################"))
	UE_LOG(LogTemp, Display, TEXT("## Player Loaded ##########################"))
	UE_LOG(LogTemp, Display, TEXT("Class: %s"), *PlayerInstance.Class->GetName())
	UE_LOG(LogTemp, Display, TEXT("Transform: %s"), *PlayerInstance.Transform.ToString())
	UE_LOG(LogTemp, Display, TEXT("IsDead: %i"), PlayerInstance.bIsDead)
	UE_LOG(LogTemp, Display, TEXT("Health: %f"), PlayerInstance.Health)
	UE_LOG(LogTemp, Display, TEXT("GunMag: %i"), PlayerInstance.GunMag)
	UE_LOG(LogTemp, Display, TEXT("###########################################"))
	
	GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Cyan, TEXT("Game Loaded."));
}

void UAcopalypsSaveGame::DestroySceneActors(TArray<AActor*>& Actors)
{
	for( AActor* Actor : Actors )
	{
		if( !Actor || !Actor->IsValidLowLevel() || Actor->ActorHasTag("Player") ) continue;
		
		if( Actor->GetClass() == EnemyClass )
		{
			AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(Actor);
			Enemy->Gun->Owner = nullptr;
			Enemy->DetachFromControllerPendingDestroy();
			Enemy->Destroy();
		}
		else if( Actor->GetClass() == StaticMeshClass && Actor->IsRootComponentMovable() )
		{
			Actor->Destroy();
		}
		else if( ClassesToDelete.Find(Actor->GetClass()) != INDEX_NONE && Actor->Owner.GetClass() != PlayerClass )
		{
			Actor->Destroy();
		}
	}
}
