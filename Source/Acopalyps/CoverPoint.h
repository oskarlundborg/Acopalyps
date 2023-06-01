// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */
/** @author Isabel Mirella Diaz Johansson */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"
#include "CoverPoint.generated.h"

class AEnemyAICharacter;
class UBoxComponent;
UCLASS()
class ACOPALYPS_API ACoverPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACoverPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** True if enemy is currently taking cover behind point*/
	bool bIsOccupied = false;

	UPROPERTY(EditAnywhere)
	class AAcopalypsCharacter* PlayerCharacter;

	/** Pointer to the last enemy character instance that has occupied cover point*/
	AEnemyAICharacter* LastVisitedCharacter;

	/** Method calculating distance to player actor
	 * @return distance between point and player actor current location
	 */
	float DistanceToPlayer() const;

	/** Performs a line trace to check if enemy can see player from cover point location*/
	bool HasLineOfSightToPlayer() const;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* CoverBox;

};
