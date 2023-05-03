// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"
#include "EngineUtils.h"

UAcopalypsSaveGame::UAcopalypsSaveGame() :
	SaveSlotName("TestSaveSlot"),
	UserIndex(0)
{
	//SaveGameInstances();
}

UAcopalypsSaveGame::UAcopalypsSaveGame(FString SaveSlotName) :
	SaveSlotName(SaveSlotName),
	UserIndex(0)
{
	//SaveGameInstances();
}

void UAcopalypsSaveGame::SaveGameInstances()
{
	SavedInstances.Add(GetWorld()->GetGameInstance());
	for( AController* Controller : TActorRange<AController>(GetWorld()) )
	{
		//SavedInstances.Add(Controller->GetGameInstance());
	}
}