
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NonHostileAICharacter.generated.h"


class ACombatManager;
UCLASS()
class ACOPALYPS_API ANonHostileAICharacter : public ACharacter
{
	GENERATED_BODY()
	
	void UnRagDoll();
		
	/** Pawn mesh: 3st person view */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* CharacterMesh;

	UPROPERTY(VisibleAnywhere, Category=Health)
	class UHealthComponent* HealthComponent;

public:
	// Sets default values for this character's properties
	ANonHostileAICharacter();

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

	void RagDoll();

	void RagDoll(FVector ForceDirection);

	//ACombatManager* Manager;
};
