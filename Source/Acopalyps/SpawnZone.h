// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnZone.generated.h"

class UBoxComponent;
class AEnemyAICharacter;
UCLASS()
class ACOPALYPS_API ASpawnZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> SpawnPoints = TArray<AActor*>();

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* SpawnZoneZone;

	//void SpawnEnemies(int EnemiesToSpawn);
	
	UPROPERTY(EditAnywhere, Category="Spawning")
	int SpawnDelay;


	UPROPERTY(EditAnywhere)
	int SpawnZoneID;
};
