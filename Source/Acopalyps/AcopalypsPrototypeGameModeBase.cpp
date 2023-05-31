// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */


#include "AcopalypsPrototypeGameModeBase.h"
#include "AcopalypsCharacter.h"
#include "AcopalypsSaveGame.h"
#include "EngineUtils.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void AAcopalypsPrototypeGameModeBase::PawnKilled(APawn* PawnKilled)
{
	// if pawn killed has playerController as controller it is the player
	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
	//ensure(PlayerController);
	if (PlayerController)
	{
		UE_LOG(LogTemp, Display, TEXT("Player Killed"))
		UGameplayStatics::GetGameMode(this)->RestartPlayer(PlayerController);
	}
}

void AAcopalypsPrototypeGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(EWindowMode::Fullscreen);
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(70);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(true);
	UGameUserSettings::GetGameUserSettings()->ApplyHardwareBenchmarkResults();
}

void AAcopalypsPrototypeGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//SpawnPoint = GetWorld()->GetFirstPlayerController()->GetSpawnLocation();
	if( !OnPlayerDied.IsBound() )
	{
		OnPlayerDied.AddDynamic(this, &AAcopalypsPrototypeGameModeBase::PlayerDied);
	}
}

void AAcopalypsPrototypeGameModeBase::RestartPlayer(AController* NewPlayer)
{
	UE_LOG(LogTemp, Display, TEXT("Calling RestartPlayer"))
	Super::RestartPlayer(NewPlayer);
}

void AAcopalypsPrototypeGameModeBase::PlayerDied(ACharacter* Character)
{
	UE_LOG(LogTemp, Display, TEXT("Calling PlayerDied"))
	if( AController* CharacterController = Character->GetController() )
	{
		RestartPlayer(CharacterController);
	}
}

void AAcopalypsPrototypeGameModeBase::EndGame(bool bPlayerWon)
{
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		Controller->GameHasEnded(Controller->GetPawn(), Controller->IsPlayerController() == bPlayerWon);
		if( Controller->ActorHasTag("Player") )
		{
			//Cast<AAcopalypsCharacter>(Controller->GetPawn())->Respawn();
			RestartPlayer(Controller);
		}
	}
}

void AAcopalypsPrototypeGameModeBase::SaveLevelData() {}
