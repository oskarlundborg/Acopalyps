// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelSpawner.generated.h"

UCLASS()
class ACOPALYPS_API ALevelSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelSpawner();
	
	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<UWorld>> SubLevels;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> LevelToLoad;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> LevelToUnload;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
