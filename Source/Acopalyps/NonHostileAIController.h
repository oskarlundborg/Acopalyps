// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NonHostileAIController.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API ANonHostileAIController : public AAIController
{
	GENERATED_BODY()
public:

	ANonHostileAIController();
	
	virtual void Tick(float DeltaSeconds) override;
	void SetIsRagdoll(bool val);

	void Initialize();

	void SetRandomPoints();

protected:
	void BeginPlay() override;

private:
	//class AApocalypsCharacter* PlayerCharacter;
	APawn* PlayerPawn;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;
};
