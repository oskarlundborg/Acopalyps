// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSpawner.h"

#include "LevelStreamerSubsystem.h"

// Sets default values
ALevelSpawner::ALevelSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelSpawner::BeginPlay()
{
	Super::BeginPlay();

	GetGameInstance()->GetSubsystem<ULevelStreamerSubsystem>()->LoadLevel(SubLevels[0], 0);
}

// Called every frame
void ALevelSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

