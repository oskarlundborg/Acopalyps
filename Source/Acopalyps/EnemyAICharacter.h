// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "GameFramework/Character.h"
#include "EnemyAICharacter.generated.h"


class ACombatManager;
UCLASS()
class ACOPALYPS_API AEnemyAICharacter : public ACharacter
{
	GENERATED_BODY()

	/*
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	/** Gun object to be attached to enemy character*/
	//UPROPERTY()
	//AGun* Gun;
	void UnRagDoll();
		
	/** Pawn mesh: 3st person view */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* CharacterMesh;

	UPROPERTY(VisibleAnywhere, Category=Health)
	class UHealthComponent* HealthComponent;

	
	
public:
	// Sets default values for this character's properties
	AEnemyAICharacter();
	
	UFUNCTION(BlueprintImplementableEvent)
		void FireEnemyTriggerEvent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle RagDollTimerHandle;

	FVector LastPositionBeforeRagdoll;
	FRotator LastRotationBeforeRagdoll;

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

	ACombatManager* Manager;

	// Gun variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gun")
		TSubclassOf<class AGun> GunClass;
	UPROPERTY(BlueprintReadOnly)
		AGun* Gun;

	// Map of Ammo types and their current amount
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gun|Ammo")
		TMap<TEnumAsByte<AMMO_TYPES>, int32> AmmoCountMap = {
			// Left Barrel
			{ Regular,   100 },
			{ Bouncing,  100 },
			{ Rapid,	  300 },
			// Right Barrel
			{ Explosive, 100 },
			{ Flare,     100 },
			{ BeanBag,	  100 },
		};

};
