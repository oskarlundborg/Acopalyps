// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"

UAcopalypsSaveGame::UAcopalypsSaveGame() :
	SaveSlotName("TestSaveSlot"),
	UserIndex(0) {}

UAcopalypsSaveGame::UAcopalypsSaveGame(FString SaveSlotName) :
	SaveSlotName(SaveSlotName),
	UserIndex(0) {}
