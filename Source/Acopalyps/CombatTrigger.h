// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatTrigger.generated.h"

class ACombatManager;
class UBoxComponent;

UCLASS()
class ACOPALYPS_API ACombatTrigger : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACombatTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UBoxComponent* TriggerBox;

	ACombatManager* Manager = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bTriggerUsed = false;

private:
	UFUNCTION(BlueprintCallable,meta=(AllowPrivateAccess = true))
	void StartCombat();
};
