// Fill out your copyright notice in the Description page of Project Settings.
/**
 @author: Gin Lindelöw
*/

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "BeanBagProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API ABeanBagProjectile : public AProjectile
{
	GENERATED_BODY()
	
	ABeanBagProjectile();

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual USphereComponent* GetCollisionComp() const override;

	virtual UProjectileMovementComponent* GetProjectileMovement() const override;
};
