// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */


#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelStreamerSubsystem.generated.h"


/**
 * 
 */
UCLASS()
class ACOPALYPS_API ULevelStreamerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Load level and add level with id to LevelMap*/
	void LoadLevel(struct FLevelID SubLevelToLoad); // SoftObjectPtr = string rep of path for object/asset that may/may not be loaded

	/** Unload level and remove level with id to LevelMap */
	void UnloadLevel(int IDToUnload);
	
	/** Map with key levelID, value instanced level */
	TMap<int, class ULevelStreamingDynamic*> LevelMap;
	
};
