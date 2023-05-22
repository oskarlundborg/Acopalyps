// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"

#include "AcopalypsCharacter.h"
#include "HealthComponent.h"
#include "LevelSpawner.h" // Needed but grayed out by rider for whatever reason lol
#include "LevelStreamerSubsystem.h"
#include "Dataflow/DataflowEngineUtil.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"

class ULevelStreamingDynamic;
class ULevelStreamerSubsystem;

void UAcopalypsSaveGame::SaveGameInstance(APawn* _Player, TArray<AActor*>& InActors)
{
	WorldPtr = _Player->GetWorld();
	AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(_Player);
	
	AddInstanceRef(Player, PlayerRef);
	PlayerRef.ControllerRotation = Player->GetController()->GetControlRotation();
	PlayerRef.GunMag = Player->Gun->CurrentMag;
	PlayerRef.Health = Player->HealthComponent->GetHealth();
	PlayerRef.EquippedAmmoType = Player->Gun->CurrentAmmoType;
	PlayerRef.EquippedAltAmmoType = Player->Gun->CurrentAlternateAmmoType;
	
	SubLevels = Player->LoadedLevels;

	for( AActor* Actor : InActors )
	{
		if( SavedClasses.Find(Actor->GetClass()) != INDEX_NONE )
		{
			for( TFieldIterator<UProperty> PropIt(Actor->GetClass()); PropIt; ++PropIt )
			{
				UE_LOG(LogTemp, Display, TEXT("%s"), *PropIt->GetFName().ToString())
				if( UProperty* Val = *PropIt->ContainerPtrToValuePtr<UProperty*>(Actor) )
				{
					UE_LOG(LogTemp, Display, TEXT("%s"), *Val->GetFName().ToString())
				}
			}
			const int Index = Instances.Emplace();
			FInstanceRef& Ref = Instances[Index];
			AddInstanceRef(Actor, Ref);
		}
	}
}

void UAcopalypsSaveGame::AddInstanceRef(AActor* Actor, FInstanceRef& Ref) const
{
	FMemoryWriter MemWriter = FMemoryWriter(Ref.Data, true);
	FSaveGameArchive Archive = FSaveGameArchive(MemWriter);

	Ref.Class = Actor->GetClass();
	Ref.Name = Actor->GetFName();
	Ref.Transform = Actor->GetTransform();
	Ref.Lifespan = Actor->GetLifeSpan();
	Actor->Serialize(Archive);

	TArray<UActorComponent*> Components = TArray<UActorComponent*>();
	Actor->GetComponents(Components);
	for( UActorComponent* Comp : Components )
	{
		const int Index = Ref.Components.Emplace();
		FInstanceComponent& CompRef = Ref.Components[Index];
		CompRef = {
			.Class	= Comp->GetClass(),
			.Name	= Comp->GetFName(),
		};
		if( USceneComponent* SceneComp = Cast<USceneComponent>(Comp) )
		{
			CompRef.Transform = SceneComp->GetRelativeTransform();
			if( UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Comp) )
			{
				CompRef.Velocity = PrimComp->GetPhysicsLinearVelocity();
				CompRef.AngularVelocity = PrimComp->GetPhysicsAngularVelocityInDegrees();
			}
		}
		if( UMovementComponent* MovementComp = Cast<UMovementComponent>(Comp) )
		{
			CompRef.Velocity = MovementComp->Velocity;
		}

		FMemoryWriter CompMemWriter = FMemoryWriter(CompRef.Data, true);
		FSaveGameArchive CompArchive = FSaveGameArchive(MemWriter);
		Comp->Serialize(CompArchive);
	}
}

void UAcopalypsSaveGame::LoadGameInstance(const UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if( World != WorldPtr )
	{
		UGameplayStatics::LoadStreamLevel(
			WorldContextObject,
			WorldPtr->GetFName(),
			true,
			true,
			FLatentActionInfo()
			);
	}
	
	// Set player status
	AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
	FMemoryReader MemReader = FMemoryReader(PlayerRef.Data, true);
	FSaveGameArchive Archive = FSaveGameArchive(MemReader);
	Player->Serialize(Archive);
	//Player->SetActorTransform(PlayerRef.Self.Transform);
	//Player->GetController()->SetControlRotation(PlayerRef.ControllerRotation);
	//Player->GetMovementComponent()->Velocity = PlayerRef.Self.Velocity;
	//Player->Gun->CurrentMag = PlayerRef.GunMag;
	//Player->HealthComponent->SetHealth(PlayerRef.Health);

	// Destroy all saved actor instances in world.
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), Actors);
	for( AActor* Actor : Actors )
	{
		if( SavedClasses.Find(Actor->GetClass()) != INDEX_NONE )
		{
			UE_LOG(LogTemp, Display, TEXT("Destroying: %s"), *Actor->GetFName().ToString())
			Actor->Destroy();
		}
	}
	ULevelStreamerSubsystem* LevelStreamSubsystem = World->GetGameInstance()->GetSubsystem<ULevelStreamerSubsystem>();
	TArray<int> LevelKeys;
	LevelStreamSubsystem->UnloadLevel(LevelStreamSubsystem->LevelMap.GetKeys(LevelKeys));
	// Unload all loaded levels
	for( const int Level : LevelKeys )
	{
		LevelStreamSubsystem->UnloadLevel(Level);
	}
	// Load all levels from save
	for( int i = 0; i < SubLevels.Num(); i++ )
	{
		UE_LOG(LogTemp, Display, TEXT("%i"), SubLevels[i].ID )
		LevelStreamSubsystem->LoadLevel(SubLevels[i]);
	}

	// Create new instances of saved actors.
	for( FInstanceRef& Ref : Instances )
	{
		LoadInstanceRef(World, Ref);
		UE_LOG(LogTemp, Display, TEXT("Load: %s"), *Ref.Class->GetFName().ToString() )
	}
	for( FInstanceRef& Ref : Instances )
	{
		FinishLoadingInstanceRef(Ref);
		UE_LOG(LogTemp, Display, TEXT("Finish loading: %s"), *Ref.Class->GetFName().ToString() )
	}
}

void UAcopalypsSaveGame::LoadInstanceRef(UWorld* World, FInstanceRef& Ref) const
{
	auto Actor = World->SpawnActorDeferred<AActor>(
		Ref.Class,
		Ref.Transform
		);
	Actor->SetLifeSpan(Ref.Lifespan);

	FMemoryReader MemReader = FMemoryReader(Ref.Data, true);
	FSaveGameArchive Archive = FSaveGameArchive(MemReader);
	Actor->Serialize(Archive);

	Ref.SpawnedActor = Actor;
}

void UAcopalypsSaveGame::FinishLoadingInstanceRef(FInstanceRef& Ref) const
{
	AActor* Actor = Ref.SpawnedActor;
	if( Actor != nullptr )
	{
		TArray<UActorComponent*> Comps = TArray<UActorComponent*>();
		Actor->GetComponents(Comps);
		for( FInstanceComponent& CompRef : Ref.Components )
		{
			for( UActorComponent* Comp : Comps )
			{
				if( Comp->GetFName() == CompRef.Name )
				{
					UE_LOG(LogTemp, Display, TEXT("%s: %s"), *Actor->GetFName().ToString(),*Comp->GetFName().ToString() )
					FMemoryReader CompMemReader = FMemoryReader(CompRef.Data, true);
					FSaveGameArchive CompArchive = FSaveGameArchive(CompMemReader);
					Comp->Serialize(CompArchive);

					if( USceneComponent* SceneComponent = Cast<USceneComponent>(Comp) )
					{
						SceneComponent->SetRelativeTransform(
							CompRef.Transform,
							false,
							nullptr,
							ETeleportType::TeleportPhysics
							);
						UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Comp);
						if( PrimitiveComponent != nullptr && PrimitiveComponent->IsAnySimulatingPhysics() )
						{
							PrimitiveComponent->SetPhysicsLinearVelocity(CompRef.Velocity);
							PrimitiveComponent->SetPhysicsAngularVelocityInDegrees(CompRef.AngularVelocity);
							PrimitiveComponent->GetBodyInstance()->UpdateMassProperties();
						}
					}
					if( UMovementComponent* MovementComponent = Cast<UMovementComponent>(Comp) )
					{
						MovementComponent->Velocity = CompRef.Velocity;
						MovementComponent->UpdateComponentVelocity();
					}
				}
			}
		}
		Actor->FinishSpawning(Ref.Transform);
	}
}

/*
void UAcopalypsSaveGame::SaveGameInstance(const UWorld* World, TArray<AActor*> Actors)
{
	// Store data from actors in scene
	for( AActor* Actor : Actors )
	{
		if( !IsValid(Actor) ) continue;
		
		// Get iteration's actor class
		const auto ActorClass = Actor->GetClass();
		UE_LOG(LogTemp, Display, TEXT("%s"), *ActorClass->GetName())
		
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
			TArray<uint8> TData;
			FMemoryWriter MemWriter = FMemoryWriter(TData);
			FSaveGameArchive Ar = FSaveGameArchive(MemWriter);
			Actor->Serialize(Ar);
			InstancesInWorld.Add({
				.Name		= Player->GetFName(),
				.Class		= ActorClass,
				.Transform	= Player->GetTransform(),
				.Data		= TData,
				.PlayerData	= {
                	.CameraRotation	= Player->GetController()->GetControlRotation(),
					.Velocity		= Player->GetVelocity(),
                	.bIsDead		= Player->HealthComponent->IsDead(),
                	.Health			= Player->HealthComponent->GetHealth(),
                	.GunMag			= Player->Gun->CurrentMag,
                },
			});
			SubLevels = Player->LoadedLevels;
		}
		// Save enemy specific data
		else if( SavedClasses.Find(ActorClass) != INDEX_NONE )
		{
			bool bIsDead = false;
			if( const AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(Actor) )
			{
				bIsDead = Enemy->IsDead();
			}
			else if( const AEnemyDroneBaseActor* Drone = Cast<AEnemyDroneBaseActor>(Actor) )
			{
				bIsDead = Drone->IsDead();
			}
			TArray<uint8> TData;
			FMemoryWriter MemWriter = FMemoryWriter(TData);
			FSaveGameArchive Ar = FSaveGameArchive(MemWriter);
			Actor->Serialize(Ar);
			InstancesInWorld.Add({
				.Name		= Actor->GetFName(),
				.Class		= ActorClass,
				.Transform	= Actor->GetTransform(),
				.Data		= TData,
				.bIsDead	= bIsDead,
			});
		}
		// Save non static actors data
		else if( ActorClass == AStaticMeshActor::StaticClass()
		      && Actor->GetRootComponent() == nullptr
		      && Actor->GetRootComponent()->IsSimulatingPhysics()
		      && Actor->Owner == nullptr
		       )
		{
			const AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor);
			TArray<uint8> TData;
			FMemoryWriter MemWriter = FMemoryWriter(TData);
			FSaveGameArchive Ar = FSaveGameArchive(MemWriter);
			Actor->Serialize(Ar);
			InstancesInWorld.Add({
				.Name		= StaticMeshActor->GetFName(),
				.Class		= ActorClass,
				.Transform	= StaticMeshActor->GetActorTransform(),
				.Data		= TData,
				.MeshData	= {
					.Mesh		= StaticMeshActor->GetStaticMeshComponent()->GetStaticMesh(),
				},
			});
		}
	}
	Timestamp = FDateTime::Now();
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
	//DestroySceneActors(Actors);
	ULevelStreamerSubsystem* LevelStreamSubsystem = World->GetGameInstance()->GetSubsystem<ULevelStreamerSubsystem>();
	for( TTuple<int, ULevelStreamingDynamic*> Level : LevelStreamSubsystem->LevelMap )
	{
		LevelStreamSubsystem->UnloadLevel(Level.Key);
	}
	for( auto Lvl : SubLevels )
	{
		UE_LOG(LogTemp, Display, TEXT("%i"), Lvl.ID )
	}
	for( int i = 0; i < SubLevels.Num(); i++ )
	{
		UE_LOG(LogTemp, Display, TEXT("%i"), SubLevels[i].ID )
		LevelStreamSubsystem->LoadLevel(SubLevels[i]);
	}
	for( AActor* Actor : Actors )
    {
    	if( SavedClasses.Find(Actor->GetClass()) != INDEX_NONE )
    	{
    		Actor->Destroy();
    	}
    }
	for( const FInstance Inst : InstancesInWorld )
	{
		if( Inst.Class == PlayerClass && !Inst.bIsDead )
		{
			AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(World->GetFirstPlayerController()->GetCharacter());
			//Player->SetActorTransform(Inst.Transform);
			//Player->GetMovementComponent()->Velocity = Inst.PlayerData.Velocity;
			//Player->GetController()->SetControlRotation(Inst.PlayerData.CameraRotation);
			//Player->HealthComponent->SetHealth(Inst.PlayerData.Health);
			//Player->Gun->CurrentMag = Inst.PlayerData.GunMag;
			FMemoryReader MemReader(Inst.Data);
			FSaveGameArchive Ar(MemReader);
			Player->Serialize(Ar);
		}
		else if( !Inst.bIsDead )
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Name = Inst.Name;
			SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
			auto Actor = World->SpawnActor(
				Inst.Class,
				&Inst.Transform,
				FActorSpawnParameters()
				);
			check(Actor)
			FMemoryReader MemReader(Inst.Data);
			FSaveGameArchive Ar(MemReader);
			Actor->Serialize(Ar);
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Cyan, TEXT("Game Loaded."));
}

void UAcopalypsSaveGame::DestroySceneActors(TArray<AActor*>& Actors)
{
	for( AActor* Actor : Actors )
	{
		if( !Actor || !Actor->IsValidLowLevel() || Actor->ActorHasTag("Player") ) continue;
		
		if( Actor->GetClass() == AEnemyAICharacter::StaticClass() )
		{
			AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(Actor);
			Enemy->Gun->Owner = nullptr;
			Enemy->DetachFromControllerPendingDestroy();
			Enemy->Destroy();
		}
		else if( Actor->GetClass() == AStaticMeshActor::StaticClass() && Actor->IsRootComponentMovable() )
		{
			Actor->Destroy();
		}
		else if( SavedClasses.Find(Actor->GetClass()) != INDEX_NONE
			&& Actor->Owner.GetClass() != PlayerClass
			&& Actor->IsRootComponentMovable() )
		{
			Actor->Destroy();
		}
	}
}
*/