// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SerializationSaveGame.generated.h"

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
};

/**
 * 
 */
UCLASS()
class ACOPALYPS_API USerializationSaveGame : public USaveGame
{
	GENERATED_BODY()
	
};
