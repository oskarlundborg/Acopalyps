// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatManager.h"
#include "GameFramework/Actor.h"
#include "EnemyDroneBaseActor.generated.h"

UCLASS()
class ACOPALYPS_API AEnemyDroneBaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyDroneBaseActor();

	ACombatManager* CombatManager;

	UPROPERTY(VisibleAnywhere, Category=Health)
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, Category=Collision)
	class USphereComponent* SphereColliderComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Collision)
	UStaticMeshComponent* DroneMesh;
	
	/** Called when drone starts overlapping something */
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/** Event when drone is prepared to attack player */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPrepareForAttackEvent();

	/** Event when drone starts attack*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackEvent();

	/** Event when drone stops attack*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnRetreatEvent();

	/** Method for blueprint when drone is flying*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnDroneMovement();

	/** Method for blueprint when drone is flying*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeathEvent();
	
	/** Returns if actor is dead */
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	/** Returns if actor is dead */
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	/** Called upon when object channel weapon collider collides with enemy char */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UPROPERTY(EditAnywhere)
	float InitialSpeed = 600.f;

	UPROPERTY(EditAnywhere)
	float AttackSpeed = 800.f;

	UPROPERTY(EditAnywhere)
	bool DebugAssist = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
private:
	
	bool bAttack;
	bool bIdle;

	bool bHasMetPlayer;
	
	UPROPERTY(EditAnywhere)
	class AAcopalypsCharacter* PlayerCharacter;

	
	FVector Direction;

	/** Location the player currently moves towards*/
	FVector CurrentTargetLocation;

	/** Location from which to start attack from*/
	FVector EngagedLocation;

	/** Location to move to if player is too far away*/
	FVector HomeLocation;

	/** Location to move towards after attacking player*/
	FVector RetreatLocation;

	FVector RelativePositionToPLayer;

	/** Location to move towards after attack*/
	FVector FirstEncounterLocation; // KANSKE TA BORT

	/** Location to check collision agains*/  
	FVector CollisionCheckLocation;

	/** Player character location and rotation*/
	FVector PlayerLocation;
	FRotator PlayerRotation;
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess = true))
	float CurrentSpeed = 0;

	/** Defines how far above colliding object drone moves to avoid collision*/
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess = true))
	float CollisionAvoidanceOffset = 200.f;

	/** Define the max distance to player before drone returns to home location */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float MaxDistanceToPlayer = 8000.0f;
	
	/** Define the min height of drones attack area bounds relative to the player character */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float MinHeightAbovePlayer = 300.0f;

	/** Define the max height of drones attack area bounds relative to the player character */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float MaxHeightAbovePlayer = 500.0f; 

	/** Define the min and max distance of drones attack area bounds relative to the player character */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float OuterBoundRadius = 700.0f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float InnerBoundRadius = 300.0f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float DroneRadius = 50.0f;

	/** Attack area center */
	FVector BoundCenterPosition;

	/** Time delays for tick functions*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double UpdateEngagedLocationDelay = 0.2f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double CheckAttackPotentialDelay = 0.2f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double UpdateCurrentObjectiveDelay = 0.2f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double CheckCollisionDelay = 0.2f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double CheckPlayerDistanceDelay= 1.f;

	/** Time delay before attack */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double AttackDelay = 1.2f;

	/** Time delay before retreat*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double RetreatDelay = 1.f;

	/** Time delay before retreat*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double ResumeDelay = 1.2f;

	/** Time delay before saving home location*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double SaveHomeDelay = 1.2f;

	/** Time delay before setting initial speed*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double ResumeSpeedDelay = 1.f;

	/* Looping timer handles*/
	FTimerHandle UpdateEngagedLocationTimerHandle;
	FTimerHandle CheckAttackBoundsTimerHandle;
	FTimerHandle UpdateCurrentObjectiveTimerHandle;
	FTimerHandle CheckPlayerDistanceTimerHandle;
	FTimerHandle CheckCollisionTimerHandle;

	/* One-shot timer handles*/
	FTimerHandle SaveHomeLocationTimerHandle;
	FTimerHandle AttackTimerHandle;
	FTimerHandle RetreatTimerHandle;
	FTimerHandle ResumeTimerHandle;
	FTimerHandle ResumeSpeedHandle;
	
	/** Closest colliding object between drone and target location*/
	FHitResult SweepHitResult;
	
	/** Starts timers*/
	void StartTimers();

	/** Stop timers*/
	void StopTimers();

	/** Moves actor towards a location in world*/
	void MoveTowardsLocation(float DeltaTime);

	/** Decides which location to move towards based on states*/
	void UpdateCurrentObjective();

	/** Calculates location from which to start engaging in combat encounter, and location to move to if player is too far away*/
	void SaveHomeLocation();

	/** Updates location from which to start attack*/
	void CalculateEngagedLocation();

	/** Generate new relative position to player*/
	void GenerateNewRelativePosition();

	/** Checks if drone location is in range to initiate attack*/
	bool IsWithinAttackArea() const;

	/** Checks if drone location is "too far" from player*/
	void HasLostPlayer();

	/** Calculates if drone should attack or follow player*/
	void CheckAttackPotential();

	/** Adjusts movement depending on collision*/
	void AdjustMovementForCollision();
	
	/** Performs a sweep trace, returns if colliding object found between target location and current location to avoid collision*/
	bool CollisionOnPathToTarget(FVector NewLocation);

	/** Calculates and returns closest location that avoids collision */
	FVector GetAdjustedLocation(FVector GoalLocation);
	
	/** Performs a ray casts, returns true if generated target location isnt inside a colliding object. Aka that movement to the point is possible */
	bool IsTargetLocationValid(FVector NewLocation) const;

	/** Runs when drone is in range of player to start attack */
	void PrepareForAttack();
	
	/** Runs when PrepareForAttack is done, starts drones unique attack pattern*/
	void Attack();

	/** Runs after attack, to get a new target position */
	void Retreat();

	/** Resumes drone to idle behavior*/
	void Resume();

	/** Sets drone speed to initial speed*/
	void ResumeInitialSpeed();
};



