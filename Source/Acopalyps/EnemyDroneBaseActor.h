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
	
	/** Called when drone starts overlapping something */
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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
	float InitialSpeed = 200.f;

	UPROPERTY(EditAnywhere)
	float AttackSpeed = 800.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
private:
	
	bool bAttackState;
	bool bIdleState;
	
	UPROPERTY(EditAnywhere)
	class AAcopalypsCharacter* PlayerCharacter;

	/** Current target location and direction*/
	FVector TargetLocation;
	FVector Direction;

	FVector RelativePositionToPLayer;

	/** Location to move towards during attack*/
	FVector AttackLocation = FVector::ZeroVector;

	/** Location to check collision agains*/  //NOT IN USE RN
	FVector CollisionCheckLocation;

	/** Player character location and rotation*/
	FVector PlayerLocation;
	FRotator PlayerRotation;

	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess = true))
	float CurrentSpeed = 0;

	/** Defines how far above colliding object drone moves to avoid collision*/
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess = true))
	float CollisionAvoidanceOffset = 200.f;
	
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
	double UpdateEngagedTargetDelay = 0.2f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double CheckDroneStateDelay = 0.2f;

	/** Time delay before attack */
	//UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double AttackDelay = 1.2f;

	/** Time delay before retreat*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double RetreatDelay = 1.f;

	/** Time delay before retreat*/
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double ResumeDelay = 1.2f;

	FTimerHandle UpdateEngagedLocationTimerHandle;
	FTimerHandle CheckAttackBoundsTimerHandle;
	FTimerHandle AttackTimerHandle;
	FTimerHandle RetreatTimerHandle;
	FTimerHandle ResumeTimerHandle;

	/** Closest colliding object between drone and target location*/
	FHitResult SweepHitResult;
	
	/** Starts timers*/
	void StartTimers();

	/** Stop timers*/
	void StopTimers();

	/** Moves actor towards a location in world*/
	void MoveTowardsLocation(float DeltaTime);

	/** Updates location from which to start attack*/
	FVector GetNewEngagedLocation();

	/** Generate new relative position to player*/
	void GenerateNewRelativePosition();
	
	/** Updates bounds for location to move toward*/
	void UpdateTargetLocation();

	/** Checks if drone location is in range to initiate attack*/
	bool IsWithinAttackArea() const;

	/** Calculates if drone should attack or follow player*/
	void CalculateCurrentState();

	/** Performs a sweep trace, returns if colliding object found between target location and current location to avoid collision*/
	bool CollisionOnPathToTarget(FVector NewLocation);

	/** Calculates and returns closest location that avoids collision */
	FVector GetAdjustedLocation();
	
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
};


