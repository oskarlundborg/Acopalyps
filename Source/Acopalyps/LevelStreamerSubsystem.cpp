// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */


#include "LevelStreamerSubsystem.h"

#include "LevelSpawner.h"
#include "Engine/LevelStreamingDynamic.h"
#include "LevelInstance/LevelInstanceSubsystem.h"

void ULevelStreamerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULevelStreamerSubsystem::LoadLevel(FLevelID SubLevelToLoad)
{
	bool bIsSuccess;
	
	
	if (LevelMap.Contains(SubLevelToLoad.ID) == false)
	{
		ULevelStreamingDynamic* Level = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
				this, SubLevelToLoad.LevelPtr, FVector::ZeroVector, FRotator::ZeroRotator, bIsSuccess);

		LevelMap.Add(SubLevelToLoad.ID, Level);
		
		UE_LOG(LogTemp, Warning, TEXT("Loading level with ID %i"));
	}
}


void ULevelStreamerSubsystem::UnloadLevel(int IDToUnload)
{
	if (LevelMap.Contains(IDToUnload))
	{
		LevelMap[IDToUnload]->SetIsRequestingUnloadAndRemoval(true);
		LevelMap.Remove(IDToUnload);
		UE_LOG(LogTemp, Warning, TEXT("Unloading level with ID %i"));
	}
}


