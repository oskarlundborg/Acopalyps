// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotgunProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AShotgunProjectile::AShotgunProjectile()
{
	//Projectile values overriden from the base Projectile class.
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->ProjectileGravityScale = 1.f;
}

//Overriden OnHit class
void AShotgunProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HitTriggerEvent(Hit);
	AActor* HitActor = Hit.GetActor();
	TArray<FOverlapResult> Overlaps;
	if( HitActor != nullptr && HitActor != this )
	{
		const AActor* ConstHitActor = HitActor;
		AttachToActor(HitActor, FAttachmentTransformRules::KeepWorldTransform, "NAME_None");
		UGameplayStatics::ApplyDamage(HitActor, 1.f, GetWorld()->GetFirstPlayerController(), this,nullptr);
	}
	if( bDrawDebugSphere )
	{
		DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Yellow,true,5);
	}
}

USphereComponent* AShotgunProjectile::GetCollisionComp() const
{
	return CollisionComp;
}

UProjectileMovementComponent* AShotgunProjectile::GetProjectileMovement() const
{
	return ProjectileMovement;
}
