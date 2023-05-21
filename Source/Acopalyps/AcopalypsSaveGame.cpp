// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"

#include "LevelSpawner.h" // Needed but grayed out by rider for whatever reason lol
#include "EnemyAICharacter.h"
#include "GameFramework/PawnMovementComponent.h"

class ULevelStreamerSubsystem;

void UAcopalypsSaveGame::SaveGameInstance(APawn* Player, TArray<AActor*>& InActors)
{
	check( Player != nullptr )
	ensure( Instances.Num() < 1 );
	
	AddInstanceRef(Player, PlayerRef);
	PlayerControllerRotation = Player->GetController()->GetControlRotation();

	for( AActor* Actor : InActors )
	{
		const int Index = Instances.Emplace();
		FInstanceRef& Ref = Instances[Index];
		AddInstanceRef(Actor, Ref);
	}
}

void UAcopalypsSaveGame::AddInstanceRef(AActor* Actor, FInstanceRef& Ref)
{
	FMemoryWriter MemWriter = FMemoryWriter(Ref.Self.Data, true);
	FSaveGameArchive Archive = FSaveGameArchive(MemWriter);

	Ref = {
		.Self = {
			.Class = Actor->GetClass(),
			.Name = Actor->GetFName(),
			.Transform = Actor->GetTransform(),
			.Lifespan = Actor->GetLifeSpan(),
		},
	};
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
		
		UE_LOG(LogTemp, Display, TEXT("%s"), *Ref.Self.Class->GetFName().ToString())
		UE_LOG(LogTemp, Display, TEXT("%s"), *Ref.Self.Name.ToString())
		UE_LOG(LogTemp, Display, TEXT("%s"), *Ref.Self.Velocity.ToString())
		UE_LOG(LogTemp, Display, TEXT("%s"), *Ref.Self.AngularVelocity.ToString())
	}
}

void UAcopalypsSaveGame::LoadGameInstance(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	//ACharacter* Player = UGameplayStatics::GetPlayerCharacter(World, 0);
	//Player->DetachFromControllerPendingDestroy();
	//Player->Destroy();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), Actors);
	for( AActor* Actor : Actors )
	{
		Actor->Destroy();
	}

	LoadInstance(World, PlayerRef);
	for( FInstanceRef& Ref : Instances )
	{
		LoadInstance(World, Ref);
	}

	//FinishLoadingInstance(PlayerRef);
	//APlayerController* Controller = UGameplayStatics::GetPlayerController(World, 0);
	//Controller->Possess(Cast<APawn>(PlayerRef.SpawnedActor));
	//Controller->SetControlRotation(PlayerControllerRotation);
	//Controller->UpdateRotation(1);

	for( FInstanceRef& Ref : Instances )
	{
		FinishLoadingInstance(Ref);
	}
}

void UAcopalypsSaveGame::LoadInstance(UWorld* World, FInstanceRef& Ref)
{
	FActorSpawnParameters Params;
	Params.Name = Ref.Self.Name;
	Params.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	Params.bDeferConstruction = true;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AActor* Actor = World->SpawnActor(
		Ref.Self.Class,
		&Ref.Self.Transform,
		Params
		);
	Actor->SetLifeSpan(Ref.Self.Lifespan);

	FMemoryReader MemReader = FMemoryReader(Ref.Self.Data, true);
	FSaveGameArchive Archive = FSaveGameArchive(MemReader);
	Actor->Serialize(Archive);

	Ref.SpawnedActor = Actor;
}

void UAcopalypsSaveGame::FinishLoadingInstance(FInstanceRef& Ref)
{
	if( Ref.SpawnedActor == nullptr ) return;
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
					FMemoryReader MemReader = FMemoryReader(CompRef.Data, true);
					FSaveGameArchive Archive = FSaveGameArchive(MemReader);
					Comp->Serialize(Archive);

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
		//Actor->FinishSpawning(Ref.Self.Transform);
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