// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "GameFramework/SaveGame.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "AcopalypsSaveGame.generated.h"

USTRUCT()
struct ACOPALYPS_API FInstanceComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	UClass* Class;
	UPROPERTY(VisibleAnywhere)
	FName Name;
	UPROPERTY(VisibleAnywhere)
	FTransform Transform;
	UPROPERTY(VisibleAnywhere)
	TArray<uint8> Data;
	
	UPROPERTY(VisibleAnywhere)
	FVector Velocity;
	UPROPERTY(VisibleAnywhere)
	FVector AngularVelocity;
	
	UPROPERTY(VisibleAnywhere)
	float Lifespan;
};

USTRUCT()
struct ACOPALYPS_API FInstanceRef
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	AActor* SpawnedActor;
	UPROPERTY(VisibleAnywhere)
	FInstanceComponent Self;
	UPROPERTY(VisibleAnywhere)
	TArray<FInstanceComponent> Components;
};

USTRUCT()
struct ACOPALYPS_API FPlayerRef : public FInstanceRef
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	float Health;
	UPROPERTY(VisibleAnywhere)
	FRotator ControllerRotation;
	UPROPERTY(VisibleAnywhere)
	int32 GunMag;
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<AMMO_TYPES> EquippedAmmoType;
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<AMMO_TYPES> EquippedAltAmmoType;
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
	void LoadGameInstance(const UObject* WorldContextObject);

private:
	UFUNCTION()
	void LoadInstanceRef(UWorld* World, FInstanceRef& Ref) const;
	UFUNCTION()
	void AddInstanceRef(AActor* Actor, FInstanceRef& Ref) const;

	UPROPERTY(VisibleAnywhere)
	FDateTime Timestamp;
	UPROPERTY(VisibleAnywhere)
	TSoftObjectPtr<UWorld> WorldPtr;
	
	UPROPERTY(VisibleAnywhere)
	FPlayerRef PlayerRef;
	
	UPROPERTY(VisibleAnywhere)
	TArray<FInstanceRef> Instances;
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> SavedClasses;
	UPROPERTY(VisibleAnywhere)
	TArray<struct FLevelID> SubLevels;
};
