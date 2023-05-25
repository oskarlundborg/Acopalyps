// Fill out your copyright notice in the Description page of Project Settings.


#include "SerializationSaveGame.h"

#include "AcopalypsCharacter.h"
#include "LevelSpawner.h"
#include "LevelStreamerSubsystem.h"
#include "Kismet/GameplayStatics.h"

bool USerializationSaveGame::Save(const AAcopalypsCharacter* Player)
{
	Timestamp = FDateTime::Now();
	
	TArray<AActor*> WorldActors;
	UGameplayStatics::GetAllActorsOfClass(Player, AActor::StaticClass(), WorldActors);
	for( AActor* Actor : WorldActors )
	{
		if( ClassFilter.Find(Actor->GetClass()) != INDEX_NONE )
		{
			const int Index = InstanceReferences.Emplace();
			FActorInstanceReference& Ref = InstanceReferences[Index];
			Ref = {
				.Class		= Actor->GetClass(),
            	.Name		= Actor->GetFName(),
            	.Transform	= Actor->GetTransform(),
			};
			SerializeActor(Ref, Actor);
			UE_LOG(LogTemp, Warning, TEXT("Save {%s}"), *Ref.Name.ToString())
		}
	}
	for( FActorInstanceReference& Inst : InstanceReferences )
	{
		UE_LOG(LogTemp, Display, TEXT("{%s, %i}"), *Inst.Name.ToString(), Inst.Data.Num())
	}
	SubLevels = Player->LoadedLevels;
	GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Green, TEXT("Game Saved!"));
	return true;
}

bool USerializationSaveGame::Load(const AAcopalypsCharacter* Player)
{
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

	TArray<AActor*> WorldActors;
	UGameplayStatics::GetAllActorsOfClass(Player, AActor::StaticClass(), WorldActors);
	for( AActor* Actor : WorldActors )
	{
		if( ClassFilter.Find(Actor->GetClass()) != INDEX_NONE )
		{
			Actor->Destroy();
		}
	}
	// Create all actors to load
	for( FActorInstanceReference& InstRef : InstanceReferences )
	{
		if( AActor* Actor = Player->GetWorld()->SpawnActorDeferred<AActor>(
				InstRef.Class,
				InstRef.Transform,
				nullptr,
				nullptr,
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			) )
		{
			
			ActorsToLoad.Add(InstRef, Actor);
			UE_LOG(LogTemp, Warning, TEXT("Start spawning {%s}"), *InstRef.Name.ToString())
		} else { UE_LOG(LogTemp, Warning, TEXT("Failed to spawn {%s}"), *InstRef.Name.ToString()) }
	}
	// Finish loading actors once everything is created
	for( auto [ InstRef, Actor ] : ActorsToLoad )
	{
		Actor->FinishSpawning(InstRef.Transform);
			Deserialize(InstRef, Actor);
		UE_LOG(LogTemp, Warning, TEXT("Finish spawning {%s}"), *InstRef.Name.ToString())
	}
	return true;
}

bool USerializationSaveGame::SerializeActor(FActorInstanceReference& Inst, AActor* Actor)
{
	FMemoryWriter MemWriter(Inst.Data, true);
	FSaveGameArchive Archive(MemWriter);
	Archive << Actor;
	MemWriter.SetIsSaving(true);
	Actor->Serialize(Archive);
	UE_LOG(LogTemp, Display, TEXT("Serialized {%s}."), *Actor->GetFName().ToString())
	return true;
}

bool USerializationSaveGame::Deserialize(FActorInstanceReference& Inst, AActor*& Actor)
{
	if( Actor != nullptr )
	{
		UE_LOG(LogTemp, Error, TEXT("Actor to deserialize was nullptr."))
		return false;
	}
	if( Inst.Data.Num() <= 0 )
	{
		UE_LOG(LogTemp, Error, TEXT("No data to read."))
		return false;
	}
	FMemoryReader MemReader(Inst.Data, true);
	FSaveGameArchive Archive(MemReader);
	MemReader.SetIsLoading(true);
	Actor->Serialize(Archive);
	UE_LOG(LogTemp, Display, TEXT("Deserialized %s."), *Actor->GetFName().ToString())
	return true;
}
