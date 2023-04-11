// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyGun.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyGun::AEnemyGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void AEnemyGun::PullTrigger()
{
	//UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(OwnerPawn)
	{
		AController* OwnerController = OwnerPawn->GetController();
		if(OwnerController)
		{
			FVector Location;
			FRotator Rotation;
			OwnerController->GetPlayerViewPoint(Location, Rotation);
			FVector End = Location + Rotation.Vector() * MaxRange;

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);
			Params.AddIgnoredActor(GetOwner());
			FHitResult HitResult;
			if(!GetWorld()->LineTraceSingleByChannel(HitResult, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params)) return;
			//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitResult.ImpactPoint);
			AActor* HitActor = HitResult.GetActor();
			if(HitActor)
			{
				FPointDamageEvent DamageEvent(Damage, HitResult, -Rotation.Vector(), nullptr);
				HitActor->TakeDamage(Damage, DamageEvent, GetOwner()->GetInstigatorController(), this);
			}
		}
	}
}

// Called when the game starts or when spawned
void AEnemyGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

