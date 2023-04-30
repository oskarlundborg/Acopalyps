// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "FlareProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API AFlareProjectile : public AProjectile
{
	GENERATED_BODY()

	AFlareProjectile();

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual USphereComponent* GetCollisionComp() const override;

	virtual UProjectileMovementComponent* GetProjectileMovement() const override;
};
