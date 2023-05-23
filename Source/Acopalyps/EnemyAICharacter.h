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
	UPROPERTY(EditAnywhere, Category=Avoidance)
	UBoxComponent* BoxAvoidance;
	
public:
	// Sets default values for this character's properties
	AEnemyAICharacter();
	
	UFUNCTION(BlueprintImplementableEvent)
	void FireEnemyTriggerEvent();

	UPROPERTY(VisibleAnywhere, Category=Health)
	class UHealthComponent* HealthComponent;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle RagDollTimerHandle;

	FVector LastPositionBeforeRagdoll;
	FRotator LastRotationBeforeRagdoll;

	bool bIsInitialized = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns if charachter is dead*/
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called upon when object channel weapon collider collides with enemy char */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void Shoot();

	void RagDoll();

	void RagDoll(FVector ForceDirection);
	
	void UnRagDoll();

	void InitializeController();

	void SetFilter(TSubclassOf<UNavigationQueryFilter> FilterToSet);

	UPROPERTY(VisibleAnywhere, SaveGame)
	ACombatManager* Manager;

	// Gun variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gun")
		TSubclassOf<class AGun> GunClass;
	UPROPERTY(BlueprintReadOnly)
		AGun* Gun;

	UPROPERTY(EditAnywhere, SaveGame)
	int32 CurrentMag = 1000;
	UPROPERTY(EditAnywhere)
	int32 MaxMagSize = 1000;
};
