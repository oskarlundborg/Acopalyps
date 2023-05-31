// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AcopalypsPrototypeGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDiedSignature, ACharacter*, Character);

/**
 * 
 */
UCLASS()
class ACOPALYPS_API AAcopalypsPrototypeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void RestartPlayer(AController* NewPlayer) override;
	
	/** Run when a pawn is killed, enemy or player
	 * @param PawnKilled pointer to the pawn that has died
	 */
	void PawnKilled(APawn* PawnKilled) const;

	/** Run when initializing game*/
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

protected:
	virtual void BeginPlay() override;

	/** Ends game depending on who won - enemy or player
	 * @param bPlayerWon true if player is the one that won the game
	 */
	void EndGame(bool bPlayerWon);

	/* Saves level bound data */
	void SaveLevelData();

	/** Checks if player character is dead
	 * @param  Character pointer to the character that has died
	 */
	UFUNCTION()
	virtual void PlayerDied(ACharacter* Character);
private:
	UPROPERTY()
	FOnPlayerDiedSignature OnPlayerDied;
	
	
};
