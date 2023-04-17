// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAICharacter.h"

AExplosiveProjectile::AExplosiveProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AExplosiveProjectile::OnHit);		// set up a notification for when this component hits something blocking

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

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AExplosiveProjectile::OnHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
	)
{
	AActor* HitActor = Hit.GetActor();
	TArray<FOverlapResult> Overlaps;
	if(HitActor != nullptr && ExplosionTrace(Overlaps) )
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
			Overlap.GetComponent()->AddImpulseAtLocation(GetVelocity() * 100.f, GetActorLocation());
		}
	}
	Destroy();
}

/** Performs a ray casts, returns true if hit is registered */
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

	DrawDebugSphere(GetWorld(), GetActorLocation(), 240, 10, FColor::Red, true, 5);
	return GetWorld()->OverlapMultiByChannel(
		Overlaps,
		GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(240),
		Params
		);
}
