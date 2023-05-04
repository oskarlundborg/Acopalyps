// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	FString SaveGameSlotName = TEXT("Test");
	UPROPERTY(BlueprintReadWrite, Category=Settings)
	class UAcopalypsSaveGame* SaveGameObject;

	UFUNCTION(BlueprintCallable, Category=GameManager)
	void LoadGame();
	UFUNCTION(BlueprintCallable, Category=GameManager)
	void SaveGame();
};