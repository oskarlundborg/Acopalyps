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
	HitActor = Hit.GetActor();
	TArray<FOverlapResult> Overlaps;
	if( HitActor != nullptr && HitActor != this )
	{
		const AActor* ConstHitActor = HitActor;
		HitTriggerEvent(ConstHitActor);
		AttachToActor(HitActor, FAttachmentTransformRules::KeepWorldTransform, "NAME_None");
		UGameplayStatics::ApplyDamage(HitActor, 20.f, GetWorld()->GetFirstPlayerController(), this,nullptr);
		GetWorldTimerManager().SetTimer(TickDamageTimerHandle, this, &AFlareProjectile::TickDamage, 1.f, true);
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AFlareProjectile::DisableTickDamage, 10.f, false);
	}
	if( bDrawDebugSphere )
	{
		DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Yellow,true,5);
	}
}

void AFlareProjectile::TickDamage()
{
	UE_LOG(LogTemp, Display, TEXT("Tick Damage on %s"), *HitActor->GetName());
	if( HitActor != nullptr )
	{
		UGameplayStatics::ApplyDamage(HitActor, 2.f, GetWorld()->GetFirstPlayerController(), this,nullptr);
	}
}


void AFlareProjectile::DisableTickDamage()
{
	ensure(false);
	UE_LOG(LogTemp, Display, TEXT("Disable and destroy"));
	GetWorldTimerManager().ClearTimer(TickDamageTimerHandle);
	Destroy();
}

USphereComponent* AFlareProjectile::GetCollisionComp() const
{
	return CollisionComp;
}

UProjectileMovementComponent* AFlareProjectile::GetProjectileMovement() const
{
	return ProjectileMovement;
}
