// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
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

/*
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
	//UPROPERTY(VisibleAnywhere)
	//TArray<FCombatWave> CombatWaves;
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
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	ACombatManager* CombatManager;
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
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	ACombatManager* CombatManager;
};

USTRUCT()
struct FInstance
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FName Name;
	UPROPERTY(VisibleAnywhere)
	UClass* Class;
	UPROPERTY(VisibleAnywhere)
	FTransform Transform;
	UPROPERTY(VisibleAnywhere)
	TArray<uint8> Data;
	
	UPROPERTY(VisibleAnywhere)
	bool bIsDead;
	
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
*/

USTRUCT()
struct ACOPALYPS_API FInstanceComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UClass* Class;
	UPROPERTY()
	FName Name;
	UPROPERTY()
	FTransform Transform;
	UPROPERTY()
	TArray<uint8> Data;
	
	UPROPERTY()
	FVector Velocity;
	UPROPERTY()
	FVector AngularVelocity;
	
	UPROPERTY()
	float Lifespan;
};

USTRUCT()
struct ACOPALYPS_API FInstanceRef
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* SpawnedActor;
	UPROPERTY()
	FInstanceComponent Self;
	UPROPERTY()
	TArray<FInstanceComponent> Components;
};

struct FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveGameArchive(FArchive& InInnerArchive) 
		: FObjectAndNameAsStringProxyArchive(InInnerArchive,true)
	{
		ArIsSaveGame = true;
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
	void SaveGameInstance(APawn* Player, TArray<AActor*>& InActors);
	UFUNCTION(meta=(WorldContext=WorldContextObject))
	void LoadGameInstance(UObject* WorldContextObject);

private:
	UFUNCTION()
	void LoadInstance(UWorld* World, FInstanceRef& Ref);
	UFUNCTION()
	void FinishLoadingInstance(FInstanceRef& Ref);
	UFUNCTION()
	void AddInstanceRef(AActor* Actor, FInstanceRef& Ref);
	
	UPROPERTY()
	FInstanceRef PlayerRef;
	UPROPERTY()
	FRotator PlayerControllerRotation;
	
	UPROPERTY()
	TArray<FInstanceRef> Instances;
/*
	UFUNCTION()
	void DestroySceneActors(TArray<AActor*>& Actors);
	
	// World Info
	UPROPERTY(VisibleAnywhere, Category=World)
	FName WorldName;
	UPROPERTY(VisibleAnywhere, Category=World)
	TSoftObjectPtr<UWorld> WorldPtr;
	UPROPERTY(VisibleAnywhere)
	FDateTime Timestamp;
	
	// Player Info
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AAcopalypsCharacter> PlayerClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<TSubclassOf<AActor>> SavedClasses;
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<TSubclassOf<AActor>> SavedClassesWithFilters;
	
	// Actors In World Info
	UPROPERTY(EditDefaultsOnly, Category="Instances")
	TArray<struct FLevelID> SubLevels;
	UPROPERTY(VisibleAnywhere, Category=LevelInfo)
	TArray<FInstance> InstancesInWorld;
	*/
};
