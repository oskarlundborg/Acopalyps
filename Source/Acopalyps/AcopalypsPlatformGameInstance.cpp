// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsPlatformGameInstance.h"

#include "AcopalypsCharacter.h"
#include "AcopalypsSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UAcopalypsPlatformGameInstance::SaveGame(TArray<AActor*>& InActors)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Saving..."));
	SaveGameObject = Cast<UAcopalypsSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	if( SaveGameObject != nullptr )
	{
		SaveGameObject->SaveGame(Cast<AAcopalypsCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()), InActors);
		if( SaveGameObject->IsValidLowLevel() )
		{
			UGameplayStatics::SaveGameToSlot(SaveGameObject, TEXT("default"), 0);
			GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Green, TEXT("Game saved."));
		}
	} else { GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red, TEXT("Error: Unable to save.")); }
}

void UAcopalypsPlatformGameInstance::LoadGame()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Loaading..."));
	if( UAcopalypsSaveGame* SaveGame = Cast<UAcopalypsSaveGame>(UGameplayStatics::LoadGameFromSlot("default", 0)) ) {
		SaveGame->LoadGame(Cast<AAcopalypsCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)));
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Green, TEXT("Game loaded."));
	} else { GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red, TEXT("Error: No game to load.")); }
}
