// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelStreamerSubsystem.h"

#include "Engine/LevelStreamingDynamic.h"
#include "LevelInstance/LevelInstanceSubsystem.h"

void ULevelStreamerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULevelStreamerSubsystem::LoadLevel(TSoftObjectPtr<UWorld> SubLevelToLoad, int ID)
{
	bool bIsSuccess;
	
	CurrentInstancedLevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
				this, SubLevelToLoad, FVector::ZeroVector, FRotator::ZeroRotator, bIsSuccess);

	ensure(LevelMap.Contains(ID) == false);
	LevelMap.Add(ID, CurrentInstancedLevel);

	UE_LOG(LogTemp, Warning, TEXT("Loading level with ID %i"), ID);

	// jämföra om det är en softptr till den instanced level jag vill komma åt
	CurrentInstancedLevel->SetIsRequestingUnloadAndRemoval(true);
}

/*
void ULevelStreamerSubsystem::UnloadLevel(TSoftObjectPtr<UWorld> SubLevelToLoad, int ID)
{
	
	
	//TSoftObjectPtr<UWorld> SoftObjPtrWorld;
	//ULevelStreamingDynamic* CurrentInstancedLevel;
	
	// Ta bort från map
	UE_LOG(LogTemp, Warning, TEXT("Unloading level with ID %i"), ID);
}
*/

