// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */

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

	UPROPERTY()
	ACombatManager* CombatManager;

	UPROPERTY(VisibleAnywhere, Category=Health)
	TSoftObjectPtr<class UHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, Category=Collision)
	class USphereComponent* SphereColliderComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Collision)
	UStaticMeshComponent* DroneMesh;
	
	/** Called when drone starts overlapping something */
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called when drone dies and hits ground */
	UFUNCTION()    
	void OnDeathGroundHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	/** Event when drone is prepared to attack player */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPrepareForAttackEvent();

	/** Event when drone starts attack*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackEvent();

	/** Event when drone stops attack*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnRetreatEvent();

	/** Event when drone hits player*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnDroneHitPlayerEvent();

	/** Event when drone is flying*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnDroneMovement();

	/** Event when drone dies */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeathEvent();

	/** Event when drone crashed on ground*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeathCrashEvent();
	
	/** Returns if actor is dead
	 * @return true if drone is dead, false if not dead
	 */
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	/** Returns actors remaining health
	 *  @return health percent in float
	 */
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	/** Called upon when object channel weapon collider collides with drone*/
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UPROPERTY(EditAnywhere)
	float InitialSpeed = 550.f;

	UPROPERTY(EditAnywhere)
	float AttackSpeed = 1000.f;

	UPROPERTY(EditAnywhere)
	bool DebugAssist = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
	float TargetSpeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
	float CurrentSpeed = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	/** boolean states*/
	UPROPERTY(VisibleAnywhere, SaveGame)
	bool bAttack;
	UPROPERTY(VisibleAnywhere, SaveGame)
	bool bIdle;
	UPROPERTY(VisibleAnywhere, SaveGame)
	bool bIsDead;
    UPROPERTY(VisibleAnywhere, SaveGame)
	bool bIsPreparingForAttack;
	
	UPROPERTY(EditAnywhere, SaveGame)
	class AAcopalypsCharacter* PlayerCharacter;

	/** Current direction drone is moving in*/
	UPROPERTY(SaveGame)
	FVector Direction;

	/** Location the player currently moves towards*/
	UPROPERTY(SaveGame)
	FVector CurrentTargetLocation;

	/** Location from which to start attack from*/
	UPROPERTY(SaveGame)
	FVector EngagedLocation;

	/** Location to move towards after attacking player*/
	UPROPERTY(SaveGame)
	FVector RetreatLocation;

	/** Location to move towards when attacking player*/
	FVector AttackLocation;

	/** Location to move towards when preparing to attack player*/
	FVector PrepareAttackLocation;

	/** Location to track while moving towards player, located withing assigned bounds*/
	UPROPERTY(SaveGame)
	FVector RelativePositionToPLayer;

	/** Location to check collision towards*/  
	UPROPERTY(SaveGame)
	FVector CollisionCheckLocation;

	/** Player character location and rotation*/
	UPROPERTY(SaveGame)
	FVector PlayerLocation;
	UPROPERTY(SaveGame)
	FRotator PlayerRotation;

	/** Defines how far above colliding object drone moves to avoid collision*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float CollisionAvoidanceOffset = 200.f;
	
	/** Defines the min height of drones attack area bounds relative to the player character */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float MinHeightAbovePlayer = 80.f;

	/** Defines the max height of drones attack area bounds relative to the player character */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float MaxHeightAbovePlayer = 220.0f;

	/** Defines the distance behind player drone is moving to during attack */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float DistanceBehindPlayer = 300.0f;

	/** Defines max distance of drones attack area bounds relative to the player character */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float OuterBoundRadius = 700.0f;

	/** Defines min distance of drones attack area bounds relative to the player character */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float InnerBoundRadius = 300.0f;

	/** Defines radius of drones hitbox*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float DroneRadius = 50.0f;

	/** Attack area center, same as player character location */
	UPROPERTY(SaveGame)
	FVector BoundCenterPosition;

	//=============== Time delays for tick functions =================//
	/** Defines how often to check if drone is within outer bounds and should prepare to attack*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double CheckAttackPotentialDelay = 0.2f;

	/** Defines how often check if drone is not attacking, and then recalculate engaged location*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double UpdateCurrentObjectiveDelay = 0.2f;

	//=============== Time delays for one shot fuction calls =================//
	/** Time delay before attack */
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double AttackDelay = 1.7f;

	/** Time delay before retreat*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double RetreatDelay = 1.7f;

	/** Time delay before resuming idle behaviour*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double ResumeDelay = 1.5f;

	/** Time delay before setting initial speed*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double ResumeSpeedDelay = 2.f;

	/** Time delay before destroying drone*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double DestructionDelay = 1.f;

	/* Looping timer handles*/
	UPROPERTY(SaveGame)
	FTimerHandle CheckAttackBoundsTimerHandle;
	UPROPERTY(SaveGame)
	FTimerHandle UpdateCurrentObjectiveTimerHandle;

	/* One-shot timer handles*/
	FTimerHandle SaveHomeLocationTimerHandle;
	UPROPERTY(SaveGame)
	FTimerHandle AttackTimerHandle;
	UPROPERTY(SaveGame)
	FTimerHandle RetreatTimerHandle;
	UPROPERTY(SaveGame)
	FTimerHandle ResumeTimerHandle;
	UPROPERTY(SaveGame)
	FTimerHandle ResumeSpeedHandle;
	UPROPERTY(SaveGame)
	FTimerHandle DestructionTimerHandle;
	
	/** Closest colliding object between drone and target location*/
	UPROPERTY(SaveGame)
	FHitResult SweepHitResult;
	
	/** Starts looping timers*/
	void StartTimers();
	
	/** Moves actor towards a location in world*/
	void MoveTowardsLocation(float DeltaTime);

	/** Decides which location to move towards based on states*/
	void UpdateCurrentObjective();
	
	/** Updates location from which to start attack*/
	void CalculateEngagedLocation();

	/** Updates location to move towards during attack*/
	void CalculateAttackLocation();

	/** Updates location to move towards during retreat*/
	void CalculatePrepareAttackLocation();

	/** Generate new relative position to player*/
	void GenerateNewRelativePosition();

	/** Checks if drone location is in range to initiate attack
	 * @return true if location is within outer bounds
	 */
	bool IsWithinAttackArea() const;
	
	/** Checks if drone location is too close to player aka within inner bounds
	 * @param LocationToCheck location to check
	 * @return true if location is within inner bounds
	 */
	bool IsWithinPlayerInnerBounds(const FVector& LocationToCheck) const;

	/** Calculates if drone should attack or follow player*/
	void CheckAttackPotential();

	/** Adjusts movement depending on collision*/
	void AdjustMovementForCollision();

	/** Performs a sphere trace for objects, checking for collision with certain object types 
	 * @param SweepStartLocation location from whitch to start sweep from
	 * @param NewLocation location to sweep towards
	 * @return true if colliding object found between target location and chosen location to sweep from to avoid collision
	 */
	bool CollisionOnPathToTarget(FVector SweepStartLocation, FVector NewLocation);

	/** Calculates and returns closest location that avoids collision
	 * @return new, adjusted location to move towards to avoid detected collision
	 */
	FVector GetAdjustedLocation();

	/** Performs a line trace, returns if location is viable, and that movement to the point is possible
	 * @param NewLocation location to check 
	 * @return true if generated location isnt inside a colliding object
	 */
	bool IsTargetLocationValid(FVector NewLocation) const;

	/** Runs when drone is in range of player to start attack */
	void PrepareForAttack();
	
	/** Runs when PrepareForAttack is done, starts drones unique attack pattern*/
	virtual void Attack();

	/** Runs after attack, to get a new target position */
	void Retreat();

	/** Resumes drone to idle behavior*/
	void Resume();

	/** Sets drone speed to initial speed*/
	void ResumeInitialSpeed();

	/** Runs when drone dies*/
	void DoDeath(); //tror att OnDeath är överskuggat nånstans, därav namnet

	/** Destroys drone after a some time*/
	void DestroyDrone();
};



