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
	
	/** Run when a pawn is killed, enemy or player*/
	void PawnKilled(APawn* PawnKilled);

	/** Run when initializing game*/
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

protected:
	virtual void BeginPlay() override;

	/** Ends game depending on who won - enemy or player*/
	void EndGame(bool bPlayerWon);

	void SaveLevelData();

	UFUNCTION()
	virtual void PlayerDied(ACharacter* Character);
private:
	UPROPERTY()
	FOnPlayerDiedSignature OnPlayerDied;
	
	
};
