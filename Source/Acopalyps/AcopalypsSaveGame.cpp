// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"

#include "AcopalypsCharacter.h"
#include "EnemyAICharacter.h"
#include "EnemyDroneBaseActor.h"
#include "SpawnPoint.h"
#include "Engine/DirectionalLight.h"
#include "Engine/Light.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

void UAcopalypsSaveGame::SaveGameInstance(const UWorld* World, TArray<AActor*> Actors)
{
	// Save level name
	WorldName = FName(World->GetName());
	
	// Store data from actors in scene
	for( AActor* Actor : Actors )
	{
		// Save player specific data
		if( Actor->IsRootComponentMovable() )
		{
			TArray<uint8> tData;
			FMemoryWriter MemWriter = FMemoryWriter(tData, true);
			FSaveGameArchive Ar = FSaveGameArchive(MemWriter);
			FInstance Tmp = {
				.Name =  Actor->GetFName(),
				.Class = Actor->GetClass(),
				.Transform = Actor->GetTransform(),
				.Data = tData,
			};
			Actor->Serialize(Ar);
			Instances.Add(Tmp);
		}
		UE_LOG(LogTemp, Display, TEXT("%s"), *Actor->GetName())
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
	//FString Names = World->GetFName().ToString();
	//Names.Append(", ");
	//Names.Append(WorldName.ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, Names);
	//// Open the saved world if different
	//if( WorldName != World->GetFName() )
	//{
	//	OnLoadLevelDelegate.BindUObject(this, &UAcopalypsSaveGame::OnPostLoadLevel);
	//	UGameplayStatics::AsyncLoadGameFromSlot(FString("default"), 0, OnLoadLevelDelegate);
	//}
	
	// Set for deletion.
	//DestroySceneActors(Actors);
	//for( AActor*& Actor : Actors )
	//{
	//	if( IgnoredClasses.Find(Actor->GetClass()) == INDEX_NONE )
	//	{
	//		Actor->Destroy();
	//	}
	//}
	FActorSpawnParameters SpawnParams;
	SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	// Recreate saved state.
	for( FInstance& Instance : Instances )
	{
		if( IgnoredClasses.Find(Instance.Class) == INDEX_NONE )
		{
			SpawnParams.Name = Instance.Name;
			FMemoryReader MemReader(Instance.Data, true);
			FSaveGameArchive Ar(MemReader);
			AActor* Actor = World->SpawnActor(
				AActor::StaticClass(),
				&Instance.Transform,
				SpawnParams
				);
			check(Actor)
			Actor->Serialize(Ar);
			Actor->SetActorTransform(Instance.Transform);
			UE_LOG(LogTemp, Display, TEXT("%s"), *Instance.Name.ToString())
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
		else if( IgnoredClasses.Find(Actor->GetClass()) != INDEX_NONE && Actor->Owner.GetClass() != AAcopalypsCharacter::StaticClass() )
		{
			Actor->Destroy();
		}
	}
}
