// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AcopalypsSaveGame.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "AcopalypsPlatformGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API UAcopalypsPlatformGameInstance : public UPlatformGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString SaveGameSlotName = TEXT("default");
	UPROPERTY(BlueprintReadWrite, Category=Settings)
	class UAcopalypsSaveGame* SaveGameObject;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAcopalypsSaveGame> SaveGameClass = UAcopalypsSaveGame::StaticClass();

	UFUNCTION(BlueprintCallable, Category=GameManager)
	void LoadGame();
	UFUNCTION(BlueprintCallable, Category=GameManager)
	void SaveGame(TArray<AActor*>& InActors);
};
