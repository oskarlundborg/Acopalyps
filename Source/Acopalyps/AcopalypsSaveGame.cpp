// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"

#include "AcopalypsCharacter.h"
#include "ConstraintsManager.h"
#include "EnemyAICharacter.h"
#include "EnemyDroneBaseActor.h"
#include "Dataflow/DataflowEdNode.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UAcopalypsSaveGame::SaveGameInstance(const UWorld* World, TArray<AActor*> Actors)
{
	// Save level name
	WorldName = FName(World->GetName());

	// Store data from actors in scene
	for( AActor* Actor : Actors )
	{
		// Get iteration's actor class
		const auto ActorClass = Actor->GetClass();
		//UE_LOG(LogTemp, Display, TEXT("Actor Class: %s"), *Actor->GetClass()->GetName())
		
		// Save player specific data
		if( ActorClass == PlayerClass )
		{
			const AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(Actor);
			//InstancesInWorld.Add({
			//	.Class		= ActorClass,
			//	.Transform	= Player->GetTransform(),
			//	//.PlayerData	= {
            //    //	.CameraRotation	= Player->GetController()->GetControlRotation(),
			//	//	.Velocity		= Player->GetVelocity(),
            //    //	.bIsDead		= Player->HealthComponent->IsDead(),
            //    //	.Health			= Player->HealthComponent->GetHealth(),
            //    //	.GunMag			= Player->Gun->CurrentMag,
            //    //},
			//});
			TArray<uint8> tData;
			FMemoryWriter MemWriter = FMemoryWriter(tData, true);
			FInstance Tmp = {
				.Class = ActorClass,
				.Transform = Actor->GetTransform(),
				.Data = tData,
			};
			Instances.Add(Tmp);
		}
		// Save enemy specific data
		else if( ActorClass == EnemyClass )
		{
			const AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(Actor);
			//InstancesInWorld.Add({
			//	.Class		= ActorClass,
			//	.Transform	= Enemy->GetTransform(),
			//	//.EnemyData	= {
            //	//	.bIsDead	= Enemy->HealthComponent->IsDead(),
            //	//	.Health		= Enemy->HealthComponent->GetHealth(),
            //	//	.GunMag		= Enemy->Gun->CurrentMag,
			//	//},
			//});
			TArray<uint8> tData;
			FMemoryWriter MemWriter = FMemoryWriter(tData, true);
			FInstance Tmp = {
				.Class = ActorClass,
				.Transform = Actor->GetTransform(),
				.Data = tData,
			};
			Instances.Add(Tmp);
		}
		else if( ActorClass == EnemyDroneClass )
		{
			const AEnemyDroneBaseActor* Drone = Cast<AEnemyDroneBaseActor>(Actor);
			//InstancesInWorld.Add({
			//	.Class		= ActorClass,
			//	.Transform	= Drone->GetTransform(),
			//	//.DroneData	= {
            //	//	.bIsDead	= Drone->HealthComponent->IsDead(),
            //	//	.Health		= Drone->HealthComponent->GetHealth(),
            //	//	.MeshComp	= Drone->DroneMesh,
			//	//},
			//});
			TArray<uint8> tData;
			FMemoryWriter MemWriter = FMemoryWriter(tData, true);
			FInstance Tmp = {
				.Class = ActorClass,
				.Transform = Drone->GetTransform(),
				.Data = tData,
			};
			Instances.Add(Tmp);
		}
		// Save non static actors data
		else if( ActorClass == StaticMeshClass && Actor->GetRootComponent() != nullptr && Actor->GetRootComponent()->IsSimulatingPhysics() && Actor->Owner == nullptr )
		{
			const AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor);
			//InstancesInWorld.Add({
			//	.Class		= ActorClass,
			//	.Transform	= StaticMeshActor->GetActorTransform(),
			//	//.MeshData	= {
			//	//	.Mesh		= StaticMeshActor->GetStaticMeshComponent()->GetStaticMesh(),
			//	//},
			//});
			TArray<uint8> tData;
			FMemoryWriter MemWriter = FMemoryWriter(tData, true);
			FInstance Tmp = {
				.Class = ActorClass,
				.Transform = StaticMeshActor->GetTransform(),
				.Data = tData,
			};
			Instances.Add(Tmp);
		}
		else if( ActorClass == CombatManagerClass )
		{
			ACombatManager* CombatManager = Cast<ACombatManager>(Actor);
			TTuple<TArray<AEnemyAICharacter*>, TArray<AEnemyDroneBaseActor*>> EnemyList = CombatManager->GetEnemyLists();
			//InstancesInWorld.Add({
			//	.Class				= ActorClass,
			//	.Transform			= CombatManager->GetTransform(),
			//	//.CombatManagerData	= {
			//	//	.ManagedEnemies = EnemyList.Get<0>(),
			//	//	.ManagedDrones	= EnemyList.Get<1>(),
			//	//	.SpawnZones		= CombatManager->GetSpawnZones(),
			//	//	.CombatTriggers = CombatManager->GetCombatTriggers(),
			//	//	.CombatWaves	= CombatManager->GetCombatWaves(),
			//	//},
			//});
			TArray<uint8> tData;
			FMemoryWriter MemWriter = FMemoryWriter(tData, true);
			FInstance Tmp = {
				.Class = ActorClass,
				.Transform = CombatManager->GetTransform(),
				.Data = tData,
			};
			Instances.Add(Tmp);
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Green, TEXT("Game Saved!"));
}

void UAcopalypsSaveGame::OnPostLoadLevel(const FString& InSlotName, const int32 InUserIndex, USaveGame* InSaveData)
{
	if( GetWorld() != nullptr )
	{
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
		Cast<UAcopalypsSaveGame>(UGameplayStatics::CreateSaveGameObject(StaticClass()))->LoadGameInstance(GetWorld(), AllActors);
	}
};


void UAcopalypsSaveGame::LoadGameInstance(UWorld* World, TArray<AActor*>& Actors)
{
	FString Names = World->GetFName().ToString();
	Names.Append(", ");
	Names.Append(WorldName.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, Names);
	// Open the saved world if different
	if( WorldName != World->GetFName() )
	{
		OnLoadLevelDelegate.BindUObject(this, &UAcopalypsSaveGame::OnPostLoadLevel);
		UGameplayStatics::AsyncLoadGameFromSlot(FString("default"), 0, OnLoadLevelDelegate);
	}
	
	// Set for deletion.
	DestroySceneActors(Actors);
	
	// Recreate saved state.
	for( auto& Instance : Instances )
	{
		FMemoryReader MemReader(Instance.Data, true);
		FSaveGameArchive Ar(MemReader);
		World->SpawnActor(
			Instance.Class,
			&Instance.Transform,
			FActorSpawnParameters()
			)->Serialize(Ar);
		// Set player data.
		//if( Instance.Class == PlayerClass )
		//{
		//	if( Instance.PlayerData.bIsDead ) break;
		//
		//	AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(World->GetFirstPlayerController()->GetCharacter());
		//	Player->SetActorTransform(Instance.Transform);
		//	Player->GetMovementComponent()->Velocity = Instance.PlayerData.Velocity;
		//	Player->GetController()->SetControlRotation(Instance.PlayerData.CameraRotation);
		//	Player->HealthComponent->SetHealth(Instance.PlayerData.Health);
		//	Player->Gun->CurrentMag = Instance.PlayerData.GunMag;
		//}
		//else if( Instance.Class == EnemyClass )
		//{
		//	// If actor is enemy, set specific data
		//	if( Instance.EnemyData.bIsDead ) break;
		//
		//	const AEnemyAICharacter* Enemy = World->SpawnActor<AEnemyAICharacter>(
		//		Instance.Class,
		//		Instance.Transform.GetLocation(),
		//		Instance.Transform.Rotator()
		//		);
		//	Enemy->HealthComponent->SetHealth(Instance.EnemyData.Health);
		//	Enemy->Gun->CurrentMag = Instance.EnemyData.GunMag;
		//}
		//else if( Instance.Class == EnemyDroneClass )
		//{
		//	if( Instance.DroneData.bIsDead ) break;

		//	AEnemyDroneBaseActor* Drone = World->SpawnActor<AEnemyDroneBaseActor>(
		//		Instance.Class,
		//		Instance.Transform.GetLocation(),
		//		Instance.Transform.Rotator()
		//		);
		//	//Drone->HealthComponent->SetHealth(Instance.DroneData.Health);
		//	//Drone->DroneMesh = Instance.DroneData.MeshComp;
		//}
		//else if( Instance.Class == StaticMeshClass ) // Handle all static mesh data in scene
		//{
		//	AStaticMeshActor* StaticMesh = World->SpawnActor<AStaticMeshActor>(
		//		Instance.Class,
		//		Instance.Transform.GetLocation(),
		//		Instance.Transform.Rotator()
		//		);
		//	StaticMesh->SetMobility(EComponentMobility::Movable);
		//	StaticMesh->GetStaticMeshComponent()->SetStaticMesh(Instance.MeshData.Mesh);
		//	StaticMesh->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
		//	StaticMesh->GetStaticMeshComponent()->SetRelativeTransform(Instance.Transform);
		//	StaticMesh->GetStaticMeshComponent()->SetSimulatePhysics(true);
		//}
		//else if( Instance.Class == CombatManagerClass )
		//{
		//	ACombatManager* CombatManager = World->SpawnActor<ACombatManager>(
		//		Instance.Class,
		//		Instance.Transform.GetLocation(),
		//		Instance.Transform.Rotator()
		//		);
		//	FCombatManagerData Data = Instance.CombatManagerData;
		//	CombatManager->SetInstance(
		//		Instance.CombatManagerData.ManagedEnemies,
		//		Instance.CombatManagerData.ManagedDrones,
		//		Instance.CombatManagerData.SpawnZones,
		//		Instance.CombatManagerData.CombatTriggers,
		//		Instance.CombatManagerData.CombatWaves
		//		);
		//}
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
