// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyProjectile::AEnemyProjectile()
{
	//Projectile values overriden from the base Projectile class.
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->ProjectileGravityScale = 1.f;
}

//Overriden OnHit class
void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HitTriggerEvent(Hit);
	AActor* HitActor = Hit.GetActor();
	TArray<FOverlapResult> Overlaps;
	if( HitActor != nullptr && HitActor != this )
	{
		AttachToActor(HitActor, FAttachmentTransformRules::KeepWorldTransform, "NAME_None");
		UGameplayStatics::ApplyDamage(HitActor, 1.f, GetWorld()->GetFirstPlayerController(), this,nullptr);
	}
	if( bDrawDebugSphere )
	{
		DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Yellow,true,5);
	}
}

USphereComponent* AEnemyProjectile::GetCollisionComp() const
{
	return CollisionComp;
}

UProjectileMovementComponent* AEnemyProjectile::GetProjectileMovement() const
{
	return ProjectileMovement;
}
