// Fill out your copyright notice in the Description page of Project Settings.

#include "BouncingProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABouncingProjectile::ABouncingProjectile()
{
	ProjectileMovement->bShouldBounce=true;
}

void ABouncingProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HitTriggerEvent(Hit);
	AActor* HitActor = Hit.GetActor();
	if(HitActor != nullptr && !HitActor->ActorHasTag(TEXT("Player")))
	{
		UGameplayStatics::ApplyPointDamage(HitActor, Damage, Hit.Location, Hit, GetWorld()->GetFirstPlayerController(), this,nullptr);
		if( bDrawDebugSphere )
		{
			DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Purple,true,5);
		}
	}
	if(Bounces==0)
	{
		ProjectileMovement->bShouldBounce=false;
		SetLifeSpan(0.5);
	}
	Bounces--;
}

void ABouncingProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if(OtherActor != nullptr && !OtherActor->ActorHasTag(TEXT("Player")))
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetWorld()->GetFirstPlayerController(), this,nullptr);
		EnemyOverlapEvent(OtherActor);
		if( bDrawDebugSphere )
		{
			DrawDebugSphere(GetWorld(),GetActorLocation(),10,10,FColor::Purple,true,5);
		}
	}
}

USphereComponent* ABouncingProjectile::GetCollisionComp() const
{
	return CollisionComp;
}

UProjectileMovementComponent* ABouncingProjectile::GetProjectileMovement() const
{
	return ProjectileMovement;
}
