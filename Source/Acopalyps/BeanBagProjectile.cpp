// Fill out your copyright notice in the Description page of Project Settings.

#include "BeanBagProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EnemyAICharacter.h"

ABeanBagProjectile::ABeanBagProjectile()
{
	//Projectile values overriden from the base Projectile class.
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->ProjectileGravityScale = 1.6f;
}

//Overriden OnHit class
void ABeanBagProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HitTriggerEvent(Hit);
	AActor* HitActor = Hit.GetActor();
	TArray<FOverlapResult> Overlaps;
	if( HitActor != nullptr && HitActor != this )
	{
		UGameplayStatics::ApplyDamage(HitActor, 1.f, GetWorld()->GetFirstPlayerController(), this,nullptr);
		
		if( OtherActor->ActorHasTag(TEXT("Enemy")) )
		{
			AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(OtherActor);
			if( Enemy && !Enemy->IsDead() )
			{
				Enemy->RagDoll(GetActorForwardVector() * 50);
			}
		}
	}
	if( bDrawDebugSphere )
	{
		DrawDebugSphere(GetWorld(),Hit.Location,15,10,FColor::White,true,5);
	}
	Mesh->SetSimulatePhysics(true);
}

USphereComponent* ABeanBagProjectile::GetCollisionComp() const
{
	return CollisionComp;
}

UProjectileMovementComponent* ABeanBagProjectile::GetProjectileMovement() const
{
	return ProjectileMovement;
}
