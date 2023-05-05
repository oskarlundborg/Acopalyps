
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "NavFilters/NavigationQueryFilter.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	

public:
	virtual void Tick(float DeltaSeconds) override;
	void SetIsRagdoll(bool val);

	void Initialize();

	void SetAim();

	bool HitTraceAtPLayerSuccess();

	
	/*
	UPROPERTY(EditAnywhere, Category = Navigation)
	TSubclassOf<UNavigationQueryFilter> MyNavigationQueryFilterClass;

	UNavigationQueryFilter* Filter;

	*/
protected:
	void BeginPlay() override;

private:
	//class AApocalypsCharacter* PlayerCharacter;
	APawn* PlayerPawn;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;

	
};
