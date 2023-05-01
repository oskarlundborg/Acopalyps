// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplosiveProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	//Projectile values overriden from the base Projectile class.
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
}

//Overriden OnHit class
void AExplosiveProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	HitTriggerEvent(Hit);
	TArray<FOverlapResult> Overlaps;
	if(HitActor != nullptr && ExplosionTrace(Overlaps) )
	{
		UGameplayStatics::ApplyRadialDamageWithFalloff(
			GetWorld(),
			80.f,
			20.f,
			Hit.Location,
			ExplosionRadius / 2,
			ExplosionRadius,
			1.f,
			nullptr, // Add explosive damage type
			{},
			this,
			GetWorld()->GetFirstPlayerController(),
			ECC_Visibility
			);
		for( FOverlapResult Overlap : Overlaps )
		{
			if( Overlap.GetComponent()->IsSimulatingPhysics() )
			{
				Overlap.GetComponent()->AddImpulseAtLocation(GetActorLocation() + GetVelocity().Size() * 20.f, GetActorLocation());
				ExplostionOverlapActorEvent(Overlap.GetActor());
			}
		}
	}
	Destroy();
}

bool AExplosiveProjectile::ExplosionTrace(TArray<FOverlapResult>& Overlaps)
{
	AController* OwnerController = GetWorld()->GetFirstPlayerController();
	if(OwnerController == nullptr)
	{
		return false;
	}
	
	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	FVector End = Location + Rotation.Vector() * 240;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(OwnerController);

	if( bDrawDebugSphere )
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), 240, 10, FColor::Red, true, 5);
	}
	return GetWorld()->OverlapMultiByChannel(
		Overlaps,
		GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(240),
		Params
		);
}

USphereComponent* AExplosiveProjectile::GetCollisionComp() const
{
	return CollisionComp;
}

UProjectileMovementComponent* AExplosiveProjectile::GetProjectileMovement() const
{
	return ProjectileMovement;
}
