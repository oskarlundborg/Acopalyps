// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsPrototypeGameModeBase.h"

#include "AcopalypsCharacter.h"
#include "EngineUtils.h"

void AAcopalypsPrototypeGameModeBase::PawnKilled(APawn* PawnKilled)
{
	// if pawn killed has playerController as controller it is the player
	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
	if (PlayerController)
	{
		EndGame(false);
		Cast<AAcopalypsCharacter>(PawnKilled)->Respawn();
	}

	// loop through all enemies and check if all enemies are dead
}

void AAcopalypsPrototypeGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//SpawnPoint = GetWorld()->GetFirstPlayerController()->GetSpawnLocation();
}

void AAcopalypsPrototypeGameModeBase::EndGame(bool bPlayerWon)
{
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		Controller->GameHasEnded(Controller->GetPawn(), Controller->IsPlayerController() == bPlayerWon);
		if( Controller->ActorHasTag("Player") )
		{
			RestartPlayer(Controller);
		}
	}
}

void AAcopalypsPrototypeGameModeBase::SaveLevelData()
{
	
}
