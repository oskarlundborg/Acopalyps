// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToAndShoot.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class ACOPALYPS_API UBTTask_MoveToAndShoot : public UBTTask_MoveTo
{
	GENERATED_BODY()



public:
	UBTTask_MoveToAndShoot(const FObjectInitializer& ObjectInitializer);

	/** Acceptable radius to player before task is finished*/
	UPROPERTY( Category = Node, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableRadiusMin = 5.f;

	UPROPERTY(Category = Node, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableRadiusMax = 20.f;

	UPROPERTY(Category = Shooting, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableShootDurationMin = 0.3f;

	UPROPERTY(Category = Shooting, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableShootDurationMax = 2.f;
	
	//UPROPERTY(Category=Node, EditAnywhere)
	//class TSubclassOf<class UNavigationQueryFilter> FilterClass;

	UPROPERTY(Category=BlackBoard, EditAnywhere)
	FBlackboardKeySelector Key;
	
	//virtual FName GetNodeIconName() const override;
	FTimerHandle ShootTimerHandle;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &ownerComp, uint8 *nodeMemory);

	/** Shoots at player for a random amount of time before continuing running*/
	void Shoot() const;

private:
	/** Time interval between shooting executed*/
	float ShootingInterval;

	/** Time to shoot*/
	float ShootingDuration;

	class AEnemyAIController* EnemyController;
};
