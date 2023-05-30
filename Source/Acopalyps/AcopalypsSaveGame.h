// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AcopalypsCharacter.h"
#include "GameFramework/SaveGame.h"
#include "AcopalypsSaveGame.generated.h"

class AEnemyDroneBaseActor;
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
	UPROPERTY(VisibleAnywhere, Category=ActorInfo)
	UStaticMeshComponent* MeshComp;
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
	void SaveGameInstance(const AAcopalypsCharacter* Player, TArray<AActor*> Actors);
	UFUNCTION()
	void LoadGameInstance(AAcopalypsCharacter* Player, TArray<AActor*>& Actors);
	
	UPROPERTY(VisibleAnywhere)
	FDateTime Timestamp;
	
	// Player Info
	UPROPERTY(EditDefaultsOnly, Category="Instances")
	FActorInstance PlayerInstance;
	
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<UClass*> ClassFilter;
	
	UPROPERTY(VisibleAnywhere, Category=LevelInfo)
	TArray<FLevelID> SubLevels;
};
