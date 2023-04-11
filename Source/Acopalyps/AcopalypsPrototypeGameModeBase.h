// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AcopalypsPrototypeGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API AAcopalypsPrototypeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	/** Run when a pawn is killed, enemy or player*/
	void PawnKilled(APawn* PawnKilled);

	/** Ends game depending on who won - enemy or player*/
	void EndGame(bool bPlayerWon);
};
