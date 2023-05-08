// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyDroneBaseActor.generated.h"

UCLASS()
class ACOPALYPS_API AEnemyDroneBaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyDroneBaseActor();

	UPROPERTY(VisibleAnywhere, Category=Health)
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, Category=Collision)
	class USphereComponent* SphereColliderComponent;

	UPROPERTY(EditAnywhere, Category=Collision)
	UStaticMeshComponent* DroneMesh;

	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Returns if charachter is dead
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;
	*/
	UPROPERTY(EditAnywhere)
	float InitialSpeed = 300.f;

	UPROPERTY(EditAnywhere)
	float AttackSpeed = 600.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
private:
	UPROPERTY(EditAnywhere)
	class AAcopalypsCharacter* PlayerCharacter;

	FVector EngageTargetLocation;

	FVector AttackTargetLocation;

	FVector PlayerLocation;

	FRotator PlayerRotation;

	FVector Direction;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float HeightAbovePlayer = 200.0f; // example height
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float DistanceFromPlayer = 500.0f; // example distance; // Set the desired distance from the player here

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double MoveToEngagedTargetDelay = 0.5f;
	//double TimeSinceLastMove = 0.f;

	FTimerHandle UpdateEngagedLocationTimerHandle;

	/** Moves actor towards the approximate location from which to start attack*/
	void MoveTowardsEngagedLocation();

	/** Updates location from which to start attack*/
	void UpdateEngagedLocation();

	/** Updates bounds for location to move toward*/
	void UpdateEngagedLocationBounds();

	/** Checks if drone location is in range to initiate attack*/
	bool IsWithinAttackArea() const;

	/** Define the distance and height of the bounds relative to the player character */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float BoundDistance = 500.0f;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float BoundHeight = 400.0f;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float BoundWidth = 200.0f;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float BoundRadius = 200.0f;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float BoundLength = 200.0f;

	/** Attack area bounds */
	FVector BoundTopPosition;
	FVector BoundBottomPosition;
	FVector BoundLeftEdge;
	FVector BoundRightEdge;


	UPROPERTY(EditAnywhere)
	class UTimelineComponent* TimeLine;

};
