// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LevelSpawner.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SerializationSaveGame.generated.h"

class AAcopalypsCharacter;

USTRUCT()
struct ACOPALYPS_API FActorInstanceReference
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	UClass* Class;
	
	UPROPERTY(VisibleAnywhere)
	FName Name;

	UPROPERTY(VisibleAnywhere)
	TArray<uint8> Data;

	UPROPERTY(VisibleAnywhere)
	FTransform Transform;

	friend FArchive& operator<<(FArchive& Ar, FActorInstanceReference& Inst)
	{
		Ar << Inst.Class;
		Ar << Inst.Name;
		Ar << Inst.Data;
		Ar << Inst.Transform;
		return Ar;
	}

	bool operator==(const FActorInstanceReference& Other) const
	{
		return Name == Other.Name
			&& Class == Other.Class
			&& &Transform == &Other.Transform
			&& Data == Other.Data;
	}
};

FORCEINLINE uint32 GetTypeHash(const FActorInstanceReference& Ref)
{
	return FCrc::MemCrc32(&Ref, sizeof(FActorInstanceReference));
}

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
class ACOPALYPS_API USerializationSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UFUNCTION()
	bool Save(const AAcopalypsCharacter* Player);
	UFUNCTION()
	bool Load(const AAcopalypsCharacter* Player);
private:
	UPROPERTY(VisibleAnywhere)
	FDateTime Timestamp;

	UFUNCTION()
	static bool SerializeActor(FActorInstanceReference& Inst, AActor* Actor);
	UFUNCTION()
	static bool Deserialize(FActorInstanceReference& Inst, AActor*& Actor);

	UPROPERTY(VisibleAnywhere)
	TArray<FActorInstanceReference> InstanceReferences;
	
	UPROPERTY(VisibleAnywhere)
	TArray<struct FLevelID> SubLevels = TArray<FLevelID>();

	UPROPERTY(VisibleAnywhere)
	TMap<FActorInstanceReference, AActor*> ActorsToLoad;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> PlayerClass;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> ClassFilter;
};
