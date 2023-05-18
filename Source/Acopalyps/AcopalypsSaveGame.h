// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CombatManager.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
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
	UPROPERTY(VisibleAnywhere)
	TArray<FCombatWave> CombatWaves;
};

USTRUCT()
struct FPlayerData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category=PlayerInfo)
	FRotator CameraRotation;
	UPROPERTY(VisibleAnywhere, Category=PlayerInfo)
	FVector Velocity;
	UPROPERTY(VisibleAnywhere, Category=PlayerInfo)
	bool bIsDead;
	UPROPERTY(VisibleAnywhere, Category=PlayerInfo)
	float Health;
	UPROPERTY(VisibleAnywhere, Category=PlayerInfo)
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
	UPROPERTY()
	TArray<uint8> Data;
};

struct FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveGameArchive(FArchive& InInnerArchive) 
		: FObjectAndNameAsStringProxyArchive(InInnerArchive,true)
	{
		ArIsSaveGame = true;
		ArNoDelta	 = true;
	}
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
private:
	UFUNCTION()
	void OnPostLoadLevel(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

	UFUNCTION()
	void DestroySceneActors(TArray<AActor*>& Actors);
	
	UPROPERTY(VisibleAnywhere)
	FString SlotName;
	UPROPERTY(VisibleAnywhere)
	uint32 UserIndex;

	// World Info
	UPROPERTY(VisibleAnywhere, Category=World)
	FName WorldName;

	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<TSubclassOf<AActor>> ClassesToUnload;
	
	// Actors In World Info
	UPROPERTY(EditDefaultsOnly, Category="Instances")
	TArray<FInstance> Instances;
	UPROPERTY(EditDefaultsOnly, Category="Instances")
	TArray<AActor*> TmpInstances;
	
	FAsyncLoadGameFromSlotDelegate OnLoadLevelDelegate;
};
