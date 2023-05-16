// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToAndShoot.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API UBTTask_MoveToAndShoot : public UBTTask_MoveTo
{
	GENERATED_BODY()



public:
	UBTTask_MoveToAndShoot(const FObjectInitializer& ObjectInitializer);

	/** Acceptable radius to player before task is finished*/
	UPROPERTY(Category = Node, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableRadiusMin;

	UPROPERTY(Category = Node, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableRadiusMax;

	UPROPERTY(Category = Shooting, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableShootDurationMin;

	UPROPERTY(Category = Shooting, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableShootDurationMax;

	UPROPERTY(Category = Shooting, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableShootIntervalMin;

	UPROPERTY(Category = Shooting, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableShootIntervalMax;

	
	//UPROPERTY(Category=Node, EditAnywhere)
	//class TSubclassOf<class UNavigationQueryFilter> FilterClass;

	UPROPERTY(Category=BlackBoard, EditAnywhere)
	FBlackboardKeySelector Key;
	
	//virtual FName GetNodeIconName() const override;
	FTimerHandle ShootTimerHandle;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &ownerComp, uint8 *nodeMemory) override;

	/** Shoots at player for a random amount of time before continuing running*/
	void Shoot();

	//virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
private:
	/** Time interval between shooting executed*/
	float ShootingInterval;

	/** Time to shoot*/
	float ShootingDuration;

	/** If func is looping*/
	bool bIsShooting;

	float ElapsedShootingTime;

	AActor* EnemyActor;
};
