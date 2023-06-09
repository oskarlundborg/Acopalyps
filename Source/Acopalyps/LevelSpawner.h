// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelSpawner.generated.h"

USTRUCT(BlueprintType)
struct FLevelID 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int ID;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> LevelPtr;
};


UCLASS()

class ACOPALYPS_API ALevelSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelSpawner();

	/** Array of level-id-structs that identifies levels to load*/
	UPROPERTY(EditAnywhere, SaveGame)
	TArray<FLevelID> SubLevelsToLoad;

	/** Array of level-id-structs that identifies levels to unload*/
	UPROPERTY(EditAnywhere, SaveGame)
	TArray<int> SubLevelsToUnload;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Trigger for loading/unloading levels*/
	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/** Overlap volume to trigger level streaming */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* OverlapVolume;

};
