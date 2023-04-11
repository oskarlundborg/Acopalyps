// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyAICharacter.generated.h"


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
	
		
	/** Pawn mesh: 3st person view */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* CharacterMesh;
	
public:
	// Sets default values for this character's properties
	AEnemyAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 50.f;

	UPROPERTY(VisibleAnywhere)
	float Health;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns if charachter is dead*/
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called upon when object channel weapon collider collides with enemy char */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void Shoot();

	void Ragdoll();

	void Unragdoll();

};
