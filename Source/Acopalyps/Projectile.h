// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ACOPALYPS_API AProjectile : public AActor
{
	GENERATED_BODY()

protected:
	
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
		USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Mesh;

public:
	AProjectile();

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	float Damage;

	/** called when projectile hits something */
	UFUNCTION()
		virtual void OnHit(
			UPrimitiveComponent* HitComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			FVector NormalImpulse,
			const FHitResult& Hit
			);

	UFUNCTION(BlueprintImplementableEvent)
		void HitTriggerEvent(const FHitResult& Hit);
	
	/** Returns CollisionComp subobject **/
	virtual USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	virtual UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	UPROPERTY(EditAnywhere)
		bool bDrawDebugSphere;

	FTimerDelegate DestructionDelegate;
	UPROPERTY()
		FTimerHandle DestructionTimer;
};
