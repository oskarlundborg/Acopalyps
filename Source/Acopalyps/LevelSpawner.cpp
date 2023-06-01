// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */


#include "LevelSpawner.h"
#include "AcopalypsCharacter.h"
#include "Components/BoxComponent.h"
#include "LevelStreamerSubsystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALevelSpawner::ALevelSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;

	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelSpawner::OverlapBegins);
}

// Called when the game starts or when spawned
void ALevelSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

/** Trigger for loading/unloading levels*/
void ALevelSpawner::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAcopalypsCharacter* PlayerCharacter = Cast<AAcopalypsCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (OtherActor == PlayerCharacter)
	{
		for (int i = 0; i < SubLevelsToLoad.Num(); i++)
		{
			GetGameInstance()->GetSubsystem<ULevelStreamerSubsystem>()->LoadLevel(SubLevelsToLoad[i]);
		}
		
		for (int i = 0; i < SubLevelsToUnload.Num(); i++)
		{
			GetGameInstance()->GetSubsystem<ULevelStreamerSubsystem>()->UnloadLevel(SubLevelsToUnload[i]);
		}
		PlayerCharacter->SetLoadedLevels(SubLevelsToLoad);
	}
}

// Called every frame
void ALevelSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

