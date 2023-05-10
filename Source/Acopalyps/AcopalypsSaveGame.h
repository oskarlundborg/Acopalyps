// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AcopalypsSaveGame.generated.h"

class ASkeletalMeshActor;
class AProjectile;
class AStaticMeshActor;
class AAcopalypsCharacter;
class AEnemyAICharacter;


USTRUCT()
struct FActorInstance
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category=ActorInfo)
	TSubclassOf<AActor> Class;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	FTransform Transform;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	FRotator Rotation;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	FVector Velocity;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	bool bIsDead;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	float Health;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	int32 GunMag;
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	UStaticMesh* Mesh;
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
	UPROPERTY(EditDefaultsOnly, Category="Instances")
	FActorInstance PlayerInstance;
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AAcopalypsCharacter> PlayerClass;

	// Enemies In World Info
	UPROPERTY(EditDefaultsOnly, Category="Instances")
	TArray<FActorInstance> EnemiesInWorld;
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AEnemyAICharacter> EnemyClass;
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AStaticMeshActor> StaticMeshClass;

	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<TSubclassOf<AActor>> ClassesToDelete;
	
	// Actors In World Info
	UPROPERTY(EditDefaultsOnly, Category="Instances")
	TArray<FActorInstance> ActorsInWorld;
};
