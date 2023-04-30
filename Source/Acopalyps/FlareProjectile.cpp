// Fill out your copyright notice in the Description page of Project Settings.


#include "FlareProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AFlareProjectile::AFlareProjectile()
{
	//Projectile values overriden from the base Projectile class.
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->ProjectileGravityScale = 1.f;
}

//Overriden OnHit class
void AFlareProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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

USphereComponent* AFlareProjectile::GetCollisionComp() const
{
	return CollisionComp;
}

UProjectileMovementComponent* AFlareProjectile::GetProjectileMovement() const
{
	return ProjectileMovement;
}
