// Copyright Epic Games, Inc. All Rights Reserved.

#include "AcopalypsGameMode.h"
#include "AcopalypsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAcopalypsGameMode::AAcopalypsGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
