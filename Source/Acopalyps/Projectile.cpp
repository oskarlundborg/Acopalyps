// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "AcopalypsCharacter.h"
#include "Gun.h"
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
	ProjectileMovement->InitialSpeed = 9000.f;
	ProjectileMovement->MaxSpeed = 9000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Mesh->SetCanEverAffectNavigation(false);
	CollisionComp->SetCanEverAffectNavigation(false);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	if( bDrawDebugSphere )
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(),10,10,FColor::Red,true,5);
	}
	CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
}

void AProjectile::OnHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
	)
{
	HitTriggerEvent(Hit);
	AActor* HitActor = Hit.GetActor();
	if(HitActor != nullptr )
	{
		UGameplayStatics::ApplyPointDamage(HitActor, Damage, Hit.Location, Hit, GetWorld()->GetFirstPlayerController(), this,nullptr);
		const AActor* ConstHitActor = HitActor;
		if( bDrawDebugSphere )
		{
			DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Cyan,true,5);
		}
	}
	Destroy();
}
