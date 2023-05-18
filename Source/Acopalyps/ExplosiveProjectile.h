// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ExplosiveProjectile.generated.h"

/**
 * 
 */

UCLASS()
class ACOPALYPS_API AExplosiveProjectile : public AProjectile
{
	GENERATED_BODY()
public:
	AExplosiveProjectile();

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
	bool ExplosionTrace(TArray<struct FOverlapResult>& Overlaps);

	UFUNCTION(BlueprintImplementableEvent)
		void ExplostionOverlapActorEvent(const AActor* Actor);
	
	virtual USphereComponent* GetCollisionComp() const override;

	virtual UProjectileMovementComponent* GetProjectileMovement() const override;

	UPROPERTY(EditAnywhere)
	float ExplosionRadius = 360.f;

	UPROPERTY(EditAnywhere)
	float GlassSmashExplosionRadius = 720.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UExplosiveDamageType> ExplosiveDamageType;
};
