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

	UPROPERTY(Category = Node, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableRadiusMin;

	UPROPERTY(Category = Node, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableRadiusMax;
	
    
	//UPROPERTY(Category=Node, EditAnywhere)
	//class TSubclassOf<class UNavigationQueryFilter> FilterClass;

	UPROPERTY(Category=BlackBoard, EditAnywhere)
	FBlackboardKeySelector Key;
	
	//virtual FName GetNodeIconName() const override;

	protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &ownerComp, uint8 *nodeMemory) override;

};
