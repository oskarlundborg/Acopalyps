// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToAndShoot.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class ACOPALYPS_API UBTTask_MoveToAndShoot : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UBTTask_MoveToAndShoot(const FObjectInitializer& ObjectInitializer);

	/** Acceptable minimal radius to player before task is finished*/
	UPROPERTY(Config, Category = Node, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableRadiusMin = 1700.f;

	/** Acceptable maximal radius to player before task is finished*/
	UPROPERTY(Config, Category = Node, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableRadiusMax = 2000.f;

	/** Acceptable minimal time before shooting*/
	UPROPERTY(Config, Category = Shooting, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableShootDurationMin = 0.5f;

	/** Acceptable maximal time before shooting*/
	UPROPERTY(Config, Category = Shooting, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableShootDurationMax = 1.f;

	/** Acceptable minimal speed when moving*/
	UPROPERTY(Config, Category = Moving, EditAnywhere, meta=(ClampMin = "0.0"))
	float MinSpeedInterval = 600.f;
	
	/** Acceptable maximal speed when moving*/
	UPROPERTY(Config, Category = Moving, EditAnywhere, meta=(ClampMin = "0.0"))
	float MaxSpeedInterval = 900.f;
	
	/** Timer for shooting method*/
	FTimerHandle ShootTimerHandle;

protected:
	/** Overriding MoveTos ExecuteTask to allow pseudorandom shooting between given interval*/
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &ownerComp, uint8 *nodeMemory) override;

	/** Overriding MoveTos PerformMoveTask to account for navqueryfilters*/
	virtual EBTNodeResult::Type PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** Shoots at player for a random amount of time before continuing running*/
	void Shoot();

private:
	/** Time interval between shooting executed*/
	float ShootingInterval;

	/** Time to shoot*/
	float ShootingDuration;

	class AEnemyAIController* EnemyController;
};
