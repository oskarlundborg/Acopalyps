// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AProjectile::OnHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
	)
{
	AActor* HitActor = Hit.GetActor();
	TArray<FOverlapResult> Overlaps;
	if(HitActor != nullptr )
	{
		UGameplayStatics::ApplyRadialDamageWithFalloff(
			GetWorld(),
			80.f,
			20.f,
			Hit.Location,
			120.f,
			240.f,
			1.f,
			nullptr,
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
			}
		}
	}
	Destroy();
}
