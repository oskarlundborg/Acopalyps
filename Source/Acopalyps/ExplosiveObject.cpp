// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveObject.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "ExplosiveDamageType.h"
#include "HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AExplosiveObject::AExplosiveObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

}

// Called when the game starts or when spawned
void AExplosiveObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosiveObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AExplosiveObject::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if(IsDead()) return DamageApplied;
	
	DamageApplied = FMath::Min(HealthComponent->GetHealth(), DamageApplied);
	HealthComponent->SetHealth(HealthComponent->GetHealth() - DamageApplied);
	UE_LOG(LogTemp, Display, TEXT("health: %f"), HealthComponent->GetHealth());

	if(IsDead())
	{
		OnDeathTriggerEvent();
		TArray<FOverlapResult> Overlaps;
		//blow up, deal radial damage.
		if(bDrawDebugSphere)
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 10, FColor::Red, true, 5);
		}
		
		UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
			100.0,
			35.0,
			GetActorLocation(),
			ExplosionRadius/2,
			ExplosionRadius,
			1.0,
			ExplosiveDamageType,
			{},
			this,
			EventInstigator,
			ECC_Visibility);
		for(FOverlapResult Overlap : Overlaps)
		{
			if(Overlap.GetComponent()->IsSimulatingPhysics())
			{
				Overlap.GetComponent()->AddImpulseAtLocation(GetActorLocation() + GetVelocity().Size() * 40.0, GetActorLocation());
				ExplosionOverlapActorEvent(Overlap.GetActor());
			}
		}
		//kill the object
		Destroy();
	}
	return DamageApplied;
}

bool AExplosiveObject::IsDead() const
{
	
	return HealthComponent->IsDead();
}

bool AExplosiveObject::ExplosionTrace(TArray<FOverlapResult>& Overlaps) const
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





