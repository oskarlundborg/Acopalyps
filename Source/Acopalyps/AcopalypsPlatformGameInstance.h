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
	FString SaveGameSlotName;
	UPROPERTY(BlueprintReadWrite, Category=Settings)
	class UAcopalypsSaveGame* SaveGameObject;

	void LoadGame();
	void SaveGame();
};
