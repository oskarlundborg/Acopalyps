// Fill out your copyright notice in the Description page of Project Settings.

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
	void LoadLevel(TSoftObjectPtr<UWorld> SubLevelToLoad, int ID); // SoftObjectPtr = string rep of path for object/asset that may/may not be loaded

	/** Unload level and remove level with id to LevelMap
	void UnloadLevel(TSoftObjectPtr<UWorld> SubLevelToLoad, int ID);
	*/
	
	UPROPERTY(EditAnywhere)
	class ULevelStreamingDynamic* CurrentInstancedLevel;

	/** Map with key levelID, value instanced level */
	TMap<int, class ULevelStreamingDynamic*> LevelMap;

private:
	// Overlap volume to trigger level streaming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* OverlapVolume;

	
};
