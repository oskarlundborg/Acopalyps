// Fill out your copyright notice in the Description page of Project Settings.


#include "CoverPoint.h"

#include "Components/BoxComponent.h"


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
	
}

// Called every frame
void ACoverPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

