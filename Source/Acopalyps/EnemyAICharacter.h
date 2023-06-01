// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "HealthComponent.h"
#include "GameFramework/Character.h"
#include "EnemyAICharacter.generated.h"

class UNavigationQueryFilter;
class UBoxComponent;
class USphereComponent;
class ACombatManager;
UCLASS()
class ACOPALYPS_API AEnemyAICharacter : public ACharacter
{
	GENERATED_BODY()
		
	/** Pawn mesh: 3st person view */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* CharacterMesh;

	/** Box collider component that has a higher cost for avoidance*/
	// currently not in use
	UPROPERTY(EditAnywhere, Category=Avoidance)
	UBoxComponent* BoxAvoidance;
	
public:
	// Sets default values for this character's properties
	AEnemyAICharacter();

	/** Event for blueprint when enemy fires gun*/
	UFUNCTION(BlueprintImplementableEvent)
	void FireEnemyTriggerEvent();

	UPROPERTY(VisibleAnywhere, Category=Health)
	class UHealthComponent* HealthComponent;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** */
	FTimerHandle RagDollTimerHandle;

	FVector LastPositionBeforeRagdoll;
	FRotator LastRotationBeforeRagdoll;

	bool bIsInitialized = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Checks health components state, if character is dead
	 * @return true if character is dead
	 */
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	/** Gets health components current health in percent
	 * @return health percent
	 */
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	/** Event for when character dies*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called upon when object channel weapon collider collides with enemy char */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Runs when character shoots towards player character, decided in Behavior tree*/
	void Shoot();

	/** Sets physics and collision of actor to rag-doll-settings, without adding force */
	void RagDoll();

	/** Sets physics and collision of actor to rag-doll-settings, and applies force on physics body
	 *@param ForceDirection Force vector to apply, magnitude indicates strength of force
	 */
	void RagDoll(FVector ForceDirection);

	/** Resets physics and collision presets to idle after ragdoll-method call*/
	void UnRagDoll();

	/** Initializes EnemyAIController*/
	void InitializeController();
	
	UPROPERTY(VisibleAnywhere, SaveGame)
	ACombatManager* Manager;

	// ======== Gun variables ========= //
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gun")
		TSubclassOf<class AGun> GunClass;

	UPROPERTY(BlueprintReadOnly)
		AGun* Gun;
	
	UPROPERTY(EditAnywhere, SaveGame)
	int32 CurrentMag = 1000;

	UPROPERTY(EditAnywhere)
	int32 MaxMagSize = 1000;
};
