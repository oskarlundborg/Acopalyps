// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"
#include "CoverPoint.generated.h"

class UBoxComponent;
UCLASS()
class ACOPALYPS_API ACoverPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACoverPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool bIsOccupied = false;

	UBoxComponent* CoverBox;
};
