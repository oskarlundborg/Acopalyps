// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "BouncingProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API ABouncingProjectile : public AProjectile
{
	GENERATED_BODY()

	ABouncingProjectile();
	
	UPROPERTY(EditAnywhere)
	int32 Bounces = 1;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual USphereComponent* GetCollisionComp() const override;

	virtual UProjectileMovementComponent* GetProjectileMovement() const override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void EnemyOverlapEvent(AActor* OtherActor);
};
