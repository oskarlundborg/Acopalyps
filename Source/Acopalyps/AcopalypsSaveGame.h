// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AcopalypsSaveGame.generated.h"

class AEnemyDroneBaseActor;
class ASkeletalMeshActor;
class AProjectile;
class AStaticMeshActor;
class AAcopalypsCharacter;
class AEnemyAICharacter;
class ASpawnZone;
class ASpawnPoint;
class ACombatTrigger;
class ACombatManager;
struct FCombatWave;

USTRUCT()
struct FMeshData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	UStaticMesh* Mesh;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	UStaticMeshComponent* MeshComp;
};

USTRUCT()
struct FCombatManagerData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<AEnemyAICharacter*> ManagedEnemies;
	UPROPERTY(VisibleAnywhere)
	TArray<AEnemyDroneBaseActor*> ManagedDrones;
	UPROPERTY(VisibleAnywhere)
	TArray<ASpawnZone*> SpawnZones;
	UPROPERTY(VisibleAnywhere)
	TArray<ACombatTrigger*> CombatTriggers;
	UPROPERTY(EditAnywhere)
	TArray<FCombatWave> CombatWaves;
	TQueue<FCombatWave> WavesQueue;
};

USTRUCT()
struct FPlayerData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category=PlayerInfo)
	FRotator CameraRotation;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	bool bIsDead;
	UPROPERTY(VisibleAnywhere, Category=PlayerInfo)
	float Health;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	int32 GunMag;
};

USTRUCT()
struct FEnemyData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	bool bIsDead;
	UPROPERTY(VisibleAnywhere, Category=PlayerInfo)
	float Health;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	int32 GunMag;
};

USTRUCT()
struct FDroneData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	bool bIsDead;
	UPROPERTY(VisibleAnywhere, Category=PlayerInfo)
	float Health;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	UStaticMeshComponent* MeshComp;
};

USTRUCT()
struct FInstance
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AActor> Class;
	UPROPERTY(VisibleAnywhere)
	FTransform Transform;
	
	UPROPERTY(VisibleAnywhere)
	FPlayerData PlayerData;
	UPROPERTY(VisibleAnywhere)
	FEnemyData EnemyData;
	UPROPERTY(VisibleAnywhere)
	FDroneData DroneData;
	UPROPERTY(VisibleAnywhere)
	FMeshData MeshData;
	UPROPERTY(VisibleAnywhere)
	FCombatManagerData CombatManagerData;
};

/**
 * 
 */
UCLASS()
class ACOPALYPS_API UAcopalypsSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void SaveGameInstance(const UWorld* World, TArray<AActor*> Actors);
	UFUNCTION()
	void LoadGameInstance(UWorld* World, TArray<AActor*>& Actors);

	UFUNCTION()
	void DestroySceneActors(TArray<AActor*>& Actors);
	
	UPROPERTY(VisibleAnywhere)
	FString SlotName;
	UPROPERTY(VisibleAnywhere)
	uint32 UserIndex;

	// World Info
	UPROPERTY(VisibleAnywhere, Category=World)
	FName WorldName;
	
	// Player Info
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AAcopalypsCharacter> PlayerClass;

	// Enemies In World Info
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AEnemyAICharacter> EnemyClass;
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AEnemyDroneBaseActor> EnemyDroneClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AStaticMeshActor> StaticMeshClass;
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AActor> ResupplyStationClass;
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AActor> AmmoPickupClass;
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AActor> CombatManagerClass;

	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<TSubclassOf<AActor>> ClassesToUnload;
	
	// Actors In World Info
	UPROPERTY(EditDefaultsOnly, Category="Instances")
	TArray<FInstance> InstancesInWorld;
};
