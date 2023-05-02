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

	UPROPERTY(VisibleAnywhere)
	FString PlayerName;

	UPROPERTY(VisibleAnywhere)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere)
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere)
	UGameInstance* Instance;

	UAcopalypsSaveGame();
	UAcopalypsSaveGame(FString SaveSlotName);
};
