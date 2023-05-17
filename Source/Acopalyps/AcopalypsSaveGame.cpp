// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"

#include "LevelSpawner.h"
#include "AcopalypsCharacter.h"
#include "EnemyAICharacter.h"
#include "EnemyAIController.h"
#include "EnemyDroneBaseActor.h"
#include "LevelStreamerSubsystem.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/TypeContainer.h"

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
			InstancesInWorld.Add({
				.Class		= ActorClass,
				.Transform	= Player->GetTransform(),
				.PlayerData	= FPlayerData {
                	.CameraRotation	= Player->GetController()->GetControlRotation(),
                	.bIsDead		= Player->HealthComponent->IsDead(),
                	.Health			= Player->HealthComponent->GetHealth(),
                	.GunMag			= Player->Gun->CurrentMag,
                },
			});
		}
		// Save enemy specific data
		else if( ActorClass == EnemyClass )
		{
			const AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(Actor);
			InstancesInWorld.Add({
				.Class		= ActorClass,
				.Transform	= Enemy->GetTransform(),
				.EnemyData	= FEnemyData {
            		.bIsDead	= Enemy->HealthComponent->IsDead(),
            		.Health		= Enemy->HealthComponent->GetHealth(),
            		.GunMag		= Enemy->Gun->CurrentMag,
				},
			});
		}
		else if( ActorClass == EnemyDroneClass )
		{
			const AEnemyDroneBaseActor* Drone = Cast<AEnemyDroneBaseActor>(Actor);
			InstancesInWorld.Add({
				.Class		= ActorClass,
				.Transform	= Drone->GetTransform(),
				.DroneData	= FDroneData {
            		.bIsDead		= Drone->HealthComponent->IsDead(),
            		.Health			= Drone->HealthComponent->GetHealth(),
            		.MeshComp		= Drone->DroneMesh,
				},
			});
		}
		// Save non static actors data
		else if( ActorClass == StaticMeshClass && Actor->GetRootComponent() != nullptr && Actor->GetRootComponent()->IsSimulatingPhysics() && Actor->Owner == nullptr )
		{
			const AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor);
			InstancesInWorld.Add({
				.Class		= ActorClass,
				.Transform	= StaticMeshActor->GetActorTransform(),
				.MeshData	= FMeshData {
					.Mesh	= StaticMeshActor->GetStaticMeshComponent()->GetStaticMesh(),
				},
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
			if( Instance.PlayerData.bIsDead ) continue;
			
			AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(World->GetFirstPlayerController()->GetCharacter());
			Player->SetActorTransform(Instance.Transform);
			Player->GetController()->SetControlRotation(Instance.PlayerData.CameraRotation);
			Player->HealthComponent->SetHealth(Instance.PlayerData.Health);
			Player->Gun->CurrentMag = Instance.PlayerData.GunMag;
		}
		// If actor is enemy, set specific data
		if( Instance.Class == EnemyClass )
		{
			if( Instance.EnemyData.bIsDead ) continue;
			
			const AEnemyAICharacter* Enemy = World->SpawnActor<AEnemyAICharacter>(
				Instance.Class,
				Instance.Transform.GetLocation(),
				Instance.Transform.Rotator()
				);
			Enemy->HealthComponent->SetHealth(Instance.EnemyData.Health);
			Enemy->Gun->CurrentMag = Instance.EnemyData.GunMag;
		}
		else if( Instance.Class == EnemyDroneClass )
		{
			if( Instance.DroneData.bIsDead ) continue;
			
			AEnemyDroneBaseActor* Drone = World->SpawnActor<AEnemyDroneBaseActor>(
				Instance.Class,
				Instance.Transform.GetLocation(),
				Instance.Transform.Rotator()
				);
			Drone->HealthComponent->SetHealth(Instance.DroneData.Health);
			Drone->DroneMesh = Instance.DroneData.MeshComp;
		}
		else if( Instance.Class == StaticMeshClass ) // Handle all static mesh data in scene
		{
			AStaticMeshActor* StaticMesh = World->SpawnActor<AStaticMeshActor>(
				Instance.Class,
				Instance.Transform.GetLocation(),
				Instance.Transform.Rotator()
				);
			StaticMesh->SetMobility(EComponentMobility::Movable);
			StaticMesh->GetStaticMeshComponent()->SetStaticMesh(Instance.MeshData.Mesh);
			StaticMesh->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
			StaticMesh->GetStaticMeshComponent()->SetRelativeTransform(Instance.Transform);
			StaticMesh->GetStaticMeshComponent()->SetSimulatePhysics(true);
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
		else if( ClassesToUnload.Find(Actor->GetClass()) != INDEX_NONE && Actor->Owner.GetClass() != PlayerClass )
		{
			Actor->Destroy();
		}
	}
}
