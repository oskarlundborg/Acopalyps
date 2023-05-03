// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AcopalypsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API UAcopalypsSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UAcopalypsSaveGame();
	
	// Player Info
	UPROPERTY(BlueprintReadWrite, Category=PlayerInfo)
	FString PlayerName;
	UPROPERTY(BlueprintReadWrite, Category=PlayerInfo)
	FTransform PlayerTransform;
};
