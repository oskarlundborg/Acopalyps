// Fill out your copyright notice in the Description page of Project Settings.
/**
 @author: Gin Lindelöw
*/

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "LevelSpawner.h"
#include "GameFramework/SaveGame.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "AcopalypsSaveGame.generated.h"

class ACombatManager;
class AEnemyDroneBaseActor;
class AEnemyAICharacter;

USTRUCT()
struct ACOPALYPS_API FInstanceRef
{
	GENERATED_BODY()

	// Actor ref for after first load
	UPROPERTY(VisibleAnywhere)
	AActor* SpawnedActor;

	// General information for actor instance
	UPROPERTY(VisibleAnywhere)
	UClass* Class;
	UPROPERTY(VisibleAnywhere)
	FName Name;
	UPROPERTY(VisibleAnywhere)
	FTransform Transform;
	UPROPERTY(VisibleAnywhere)
	TArray<uint8> Data; // Serialization data that does not want to work :))
						// Would have made everything much easier...
	// Components for actor
	TArray<FInstanceRef> Components;

	// Character specific properties
	UPROPERTY(VisibleAnywhere)
	float Health;
	UPROPERTY(VisibleAnywhere)
	bool bIsDead;
	UPROPERTY(VisibleAnywhere)
	FRotator ControllerRotation;
	UPROPERTY(VisibleAnywhere)
	int32 GunMag;
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<AMMO_TYPES> EquippedAmmoType;
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<AMMO_TYPES> EquippedAltAmmoType;

	// Misc
	UPROPERTY(VisibleAnywhere)
	FVector Velocity;
	UPROPERTY(VisibleAnywhere)
	FVector AngularVelocity;

	friend FArchive& operator<<(FArchive& Ar, FInstanceRef& Object)
	{
		Ar << Object.Data;
		Ar << Object.Transform;
		Ar << Object.SpawnedActor;
		Ar << Object.Class;
		Ar << Object.Name;
		Ar << Object.Transform;
		Ar << Object.Data;
		Ar << Object.Components;
		Ar << Object.Health;
		Ar << Object.bIsDead;
		Ar << Object.ControllerRotation;
		Ar << Object.GunMag;
		Ar << Object.EquippedAmmoType;
		Ar << Object.EquippedAltAmmoType;
		Ar << Object.Velocity;
		Ar << Object.AngularVelocity;
		
		return Ar;
	}
};

struct FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveGameArchive(FArchive& InInnerArchive) 
		: FObjectAndNameAsStringProxyArchive(InInnerArchive,true)
	{
		ArIsSaveGame = true;
		//ArNoDelta = true;
	}
};

/**
 * 
 */
UCLASS()
class ACOPALYPS_API UAcopalypsSaveGame : public USaveGame
{
	GENERATED_BODY()

	//UAcopalypsSaveGame() {};
	
public:
	UFUNCTION()
	void SaveGame(AAcopalypsCharacter* Player, TArray<AActor*>& InActors);
	
	UFUNCTION()
	void LoadGame(AAcopalypsCharacter* Player);
	
private:
	UFUNCTION()
	void UnloadInstance(const UWorld* World, AActor* Actor) const;
	UFUNCTION()
	void LoadInstance(UWorld* World, FInstanceRef& Ref) const;
	UFUNCTION()
	void FinishLoadingInstance(FInstanceRef& Ref) const;
	
	UFUNCTION()
	bool AddInstanceRef(AActor* Actor, FInstanceRef& Ref) const;

	UPROPERTY(VisibleAnywhere)
	FDateTime Timestamp; // Unused as of now
	
	UPROPERTY(VisibleAnywhere)
	FInstanceRef PlayerRef;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> EnemyGunClass;
	UPROPERTY(EditDefaultsOnly)
	TArray<UClass*> TriggerClasses;

	UPROPERTY(VisibleAnywhere)
	TArray<FInstanceRef> Instances;
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> SavedClasses;
	UPROPERTY(VisibleAnywhere)
	TArray<FLevelID> SubLevels;
};
