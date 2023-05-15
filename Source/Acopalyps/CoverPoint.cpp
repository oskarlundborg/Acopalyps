// Fill out your copyright notice in the Description page of Project Settings.


#include "CoverPoint.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "AcopalypsCharacter.h"
#include "Math/Vector.h"


// Sets default values
ACoverPoint::ACoverPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CoverBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = CoverBox;
}

// Called when the game starts or when spawned
void ACoverPoint::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<AAcopalypsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void ACoverPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ACoverPoint::DistanceToPlayer()
{
	return FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
}

bool ACoverPoint::HasLineOfSightToPlayer()
{
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 100), UGameplayStatics::GetPlayerCharacter(this, 0)->GetActorLocation(),ECC_WorldStatic);
	if(!bHit) return true;
	return UGameplayStatics::GetPlayerCharacter(this, 0) == Cast<ACharacter>(HitResult.GetActor());
}


