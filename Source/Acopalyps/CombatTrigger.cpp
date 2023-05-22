// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */

#include "CombatTrigger.h"

#include "CombatManager.h"
#include "Components/BoxComponent.h"


// Sets default values
ACombatTrigger::ACombatTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
}

// Called when the game starts or when spawned
void ACombatTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombatTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACombatTrigger::StartCombat()
{
	if(Manager)
	{
		Manager->StartCombatMode();
	}
}
