// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UAcopalypsSaveGame::UAcopalypsSaveGame() :
	SlotName("test_save"),
	UserIndex(0) {}

void UAcopalypsSaveGame::SaveActor(AActor* Actor, FObjectRef& ActorRef)
{
	FActorSpawnParameters Params;
	Params.Name = ActorRef.GetFName();
}