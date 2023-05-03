// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsPlatformGameInstance.h"
#include "AcopalypsSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UAcopalypsPlatformGameInstance::SaveGame()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("Saving game..."));
	if( UGameplayStatics::SaveGameToSlot(
			SaveGameObject,
			SaveGameSlotName,
			0
			)
		)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("Save success!"));
	} else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Error: Save failed."));
	}
}

void UAcopalypsPlatformGameInstance::LoadGame()
{
	USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SaveGameSlotName, 0);
	SaveGameObject = Cast<UAcopalypsSaveGame>(LoadedGame);
	
	if( !SaveGameObject )
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Error: No saved game found."));
	} else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("Loading..."));
	}
}
