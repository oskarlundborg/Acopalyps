// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"

#include "LevelSpawner.h"
#include "AcopalypsCharacter.h"
#include "ConstraintsManager.h"
#include "EnemyAICharacter.h"
#include "EnemyAIController.h"
#include "EnemyDroneBaseActor.h"
#include "LevelStreamerSubsystem.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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
		//UE_LOG(LogTemp, Display, TEXT("Actor Class: %s"), *Actor->GetClass()->GetName())
		
		//ULevelStreamerSubsystem* LevelStreamSubsystem = World->GetGameInstance()->GetSubsystem<ULevelStreamerSubsystem>();
		//SubLevels.Empty();
		//for( auto [ ID, LevelStreamingDynamic ] : LevelStreamSubsystem->LevelMap )
		//{
		//	SubLevels.Add({
		//		.ID = ID,
		//		.LevelPtr = LevelStreamingDynamic->GetWorldAsset(),
		//	});
		//}
		// Save player specific data
		if( ActorClass == PlayerClass )
		{
			const AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(Actor);
			TArray<uint8> tData;
			FMemoryWriter MemWriter = FMemoryWriter(tData, true);
			FSaveGameArchive Ar = FSaveGameArchive(MemWriter);
			Actor->Serialize(Ar);
			InstancesInWorld.Add({
				.Class		= ActorClass,
				.Transform	= Player->GetTransform(),
				.PlayerData	= {
                	.CameraRotation	= Player->GetController()->GetControlRotation(),
					.Velocity		= Player->GetVelocity(),
                	.bIsDead		= Player->HealthComponent->IsDead(),
                	.Health			= Player->HealthComponent->GetHealth(),
                	.GunMag			= Player->Gun->CurrentMag,
                },
				.Data				= tData,
			});
		}
		// Save enemy specific data
		else if( ActorClass == EnemyClass )
		{
			const AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(Actor);
			TArray<uint8> tData;
			FMemoryWriter MemWriter = FMemoryWriter(tData, true);
			FSaveGameArchive Ar = FSaveGameArchive(MemWriter);
			Actor->Serialize(Ar);
			InstancesInWorld.Add({
				.Class		= ActorClass,
				.Transform	= Enemy->GetTransform(),
				.EnemyData	= {
            		.bIsDead	= Enemy->HealthComponent->IsDead(),
            		.Health		= Enemy->HealthComponent->GetHealth(),
            		.GunMag		= Enemy->Gun->CurrentMag,
					.CombatManager = Enemy->Manager,
				},
				.Data				= tData,
			});
		}
		else if( ActorClass == EnemyDroneClass )
		{
			const AEnemyDroneBaseActor* Drone = Cast<AEnemyDroneBaseActor>(Actor);
			TArray<uint8> tData;
			FMemoryWriter MemWriter = FMemoryWriter(tData, true);
			FSaveGameArchive Ar = FSaveGameArchive(MemWriter);
			Actor->Serialize(Ar);
			InstancesInWorld.Add({
				.Class		= ActorClass,
				.Transform	= Drone->GetTransform(),
				.DroneData	= {
            		.bIsDead	= Drone->HealthComponent->IsDead(),
            		.Health		= Drone->HealthComponent->GetHealth(),
            		.MeshComp	= Drone->DroneMesh,
					.CombatManager = Drone->CombatManager,
				},
				.Data				= tData,
			});
		}
		// Save non static actors data
		else if( ActorClass == StaticMeshClass && Actor->GetRootComponent() != nullptr && Actor->GetRootComponent()->IsSimulatingPhysics() && Actor->Owner == nullptr )
		{
			const AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor);
			TArray<uint8> tData;
			FMemoryWriter MemWriter = FMemoryWriter(tData, true);
			FSaveGameArchive Ar = FSaveGameArchive(MemWriter);
			Actor->Serialize(Ar);
			InstancesInWorld.Add({
				.Class		= ActorClass,
				.Transform	= StaticMeshActor->GetActorTransform(),
				.MeshData	= {
					.Mesh		= StaticMeshActor->GetStaticMeshComponent()->GetStaticMesh(),
				},
				.Data				= tData,
			});
		}
		else if( ActorClass == CombatManagerClass )
		{
			ACombatManager* CombatManager = Cast<ACombatManager>(Actor);
			TTuple<TArray<AEnemyAICharacter*>, TArray<AEnemyDroneBaseActor*>> EnemyList = CombatManager->GetEnemyLists();
			TArray<uint8> tData;
			FMemoryWriter MemWriter = FMemoryWriter(tData, true);
			FSaveGameArchive Ar = FSaveGameArchive(MemWriter);
			Actor->Serialize(Ar);
			InstancesInWorld.Add({
				.Class				= ActorClass,
				.Transform			= CombatManager->GetTransform(),
				.CombatManagerData	= {
					.ManagedEnemies = EnemyList.Get<0>(),
					.ManagedDrones	= EnemyList.Get<1>(),
					.SpawnZones		= CombatManager->GetSpawnZones(),
					.CombatTriggers = CombatManager->GetCombatTriggers(),
					//.CombatWaves	= CombatManager->GetCombatWaves(),
				},
				.Data				= tData,
			});
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
	//LevelStreamSubsystem->LevelMap.GetKeys(LevelKeys);
	//for( int i = 0; i < LevelKeys.Num(); i++ )
	//{
	//	LevelStreamSubsystem->UnloadLevel(LevelKeys[i]);
	//}
	// Load all levels from save
	for( int i = 0; i < SubLevels.Num(); i++ )
	{
		UE_LOG(LogTemp, Display, TEXT("%i"), SubLevels[i].ID )
		LevelStreamSubsystem->LoadLevel(SubLevels[i]);
	}
	
	// Set for deletion.
	DestroySceneActors(Actors);
	
	//for (int i = 0; i < SubLevels.Num(); i++)
	//{
	//	Actors.Last()->GetGameInstance()->GetSubsystem<ULevelStreamerSubsystem>()->LoadLevel(SubLevels[i]);
	//}
	// Recreate saved state.
	for( const FInstance Instance : InstancesInWorld )
	{
		// Set player data.
		if( Instance.Class == PlayerClass )
		{
			if( Instance.PlayerData.bIsDead ) break;
		
			AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(World->GetFirstPlayerController()->GetCharacter());
			Player->SetActorTransform(Instance.Transform);
			Player->GetMovementComponent()->Velocity = Instance.PlayerData.Velocity;
			Player->GetController()->SetControlRotation(Instance.PlayerData.CameraRotation);
			Player->HealthComponent->SetHealth(Instance.PlayerData.Health);
			Player->Gun->CurrentMag = Instance.PlayerData.GunMag;
			FMemoryReader MemReader(Instance.Data, true);
			FSaveGameArchive Ar(MemReader);
			Player->Serialize(Ar);
		}
		else if( Instance.Class == EnemyClass )
		{
			// If actor is enemy, set specific data
			if( Instance.EnemyData.bIsDead ) break;
		
			AEnemyAICharacter* Enemy = World->SpawnActor<AEnemyAICharacter>(
				Instance.Class,
				Instance.Transform
				);
			Enemy->HealthComponent->SetHealth(Instance.EnemyData.Health);
			Enemy->Gun->CurrentMag = Instance.EnemyData.GunMag;
			FMemoryReader MemReader(Instance.Data, true);
			FSaveGameArchive Ar(MemReader);
			Enemy->Serialize(Ar);
		}
		else if( Instance.Class == EnemyDroneClass )
		{
			if( Instance.DroneData.bIsDead ) break;

			AEnemyDroneBaseActor* Drone = World->SpawnActor<AEnemyDroneBaseActor>(
				Instance.Class,
				Instance.Transform
				);
			Drone->HealthComponent->SetHealth(Instance.DroneData.Health);
			Drone->DroneMesh = Instance.DroneData.MeshComp;
			FMemoryReader MemReader(Instance.Data, true);
			FSaveGameArchive Ar(MemReader);
			Drone->Serialize(Ar);
		}
		else if( Instance.Class == StaticMeshClass ) // Handle all static mesh data in scene
		{
			AStaticMeshActor* StaticMesh = World->SpawnActor<AStaticMeshActor>(
				Instance.Class,
				Instance.Transform
				);
			StaticMesh->SetMobility(EComponentMobility::Movable);
			StaticMesh->GetStaticMeshComponent()->SetStaticMesh(Instance.MeshData.Mesh);
			StaticMesh->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
			StaticMesh->GetStaticMeshComponent()->SetRelativeTransform(Instance.Transform);
			StaticMesh->GetStaticMeshComponent()->SetSimulatePhysics(true);
			FMemoryReader MemReader(Instance.Data, true);
			FSaveGameArchive Ar(MemReader);
			StaticMesh->Serialize(Ar);
		}
		else if( Instance.Class == CombatManagerClass )
		{
			ACombatManager* CombatManager = World->SpawnActor<ACombatManager>(
				Instance.Class,
				Instance.Transform.GetLocation(),
				Instance.Transform.Rotator()
				);
			FCombatManagerData Data = Instance.CombatManagerData;
			CombatManager->SetInstance(
				Instance.CombatManagerData.ManagedEnemies,
				Instance.CombatManagerData.ManagedDrones,
				Instance.CombatManagerData.SpawnZones,
				Instance.CombatManagerData.CombatTriggers,
				TArray<FCombatWave>() // Stopped working for some reason
				);
			FMemoryReader MemReader(Instance.Data, true);
			FSaveGameArchive Ar(MemReader);
			CombatManager->Serialize(Ar);
		}
	}
	
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
		else if( ClassesToUnload.Find(Actor->GetClass()) != INDEX_NONE
			&& Actor->Owner.GetClass() != PlayerClass
			&& Actor->IsRootComponentMovable() )
		{
			Actor->Destroy();
		}
	}
}
