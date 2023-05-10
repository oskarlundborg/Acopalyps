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

	UPROPERTY(VisibleAnywhere, Category=Collision)
	USphereComponent* CollisionCheckColliderComponent;

	UPROPERTY(EditAnywhere, Category=Collision)
	UStaticMeshComponent* DroneMesh;

	/** Called when drone collider is overlapping something */
	UFUNCTION()
	void OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/** Called when drone starts overlapping something */
	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called when drone stops overlapping something */
	UFUNCTION()
	void OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Event when drone is prepared to attack player */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPrepareForAttackEvent();

	/** Event when drone starts attack*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackEvent();

	/** Event when drone starts attack*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnEndAttackEvent();

	/** Method for blueprint when drone is flying*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnDroneMovement();
	
	/** Returns if charachter is dead
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;
	*/
	UPROPERTY(EditAnywhere)
	float InitialSpeed = 500.f;

	UPROPERTY(EditAnywhere)
	float AttackSpeed = 800.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
private:

	bool bIsPreparingAttack;
	bool bIsAttacking;
	
	UPROPERTY(EditAnywhere)
	class AAcopalypsCharacter* PlayerCharacter;

	FVector TargetLocation;

	FVector PlayerLocation;

	FRotator PlayerRotation;

	FVector Direction;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess = true))
	float CurrentSpeed = 0;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float MinHeightAbovePlayer = 300.0f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float MaxHeightAbovePlayer = 500.0f; 

	/** Define the min and max distance of drones attack area bounds relative to the player character */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float OuterBoundRadius = 700.0f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float InnerBoundRadius = 300.0f;

	/** Attack area bounds */
	FVector BoundCenterPosition;

	/** Time delays for tick functions*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double UpdateEngagedTargetDelay = 2.f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double CheckDroneStateDelay = 0.8f;

	/** Time delay before attack*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double AttackDelay = 0.5f;

	FTimerHandle UpdateEngagedLocationTimerHandle;
	FTimerHandle CheckAttackBoundsTimerHandle;
	FTimerHandle AttackTimerHandle;

	/** Starts timers*/
	void StartTimers();

	/** Moves actor towards a location, depending if drone is attacking or following player*/
	void MoveTowardsLocation(float DeltaTime);

	/** Updates location from which to start attack*/
	FVector GetNewEngagedLocation() const;

	/** Updates bounds for location to move toward*/
	void UpdateEngagedLocationBounds();

	/** Checks if drone location is in range to initiate attack*/
	bool IsWithinAttackArea() const;

	/** Calculates if drone should attack or following player*/
	void CalculateCurrentState();

	/** Performs a ray casts, returns true if hit on player is registered */
	bool CheckPathHitTrace() const;

	/** Performs a ray casts, returns true if target location isnt inside a colliding object. Aka that movement to the point is possible */
	bool CheckPathToTargetLocation(FVector NewLocation) const;

	/** Runs when drone is in range of player to start attack */
	void PrepareForAttack();
	
	/** */
	virtual void Attack();
};
