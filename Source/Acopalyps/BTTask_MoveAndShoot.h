// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "BTTask_MoveAndShoot.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API UBTTask_MoveAndShoot : public UBTTaskNode
{
	GENERATED_BODY()

	
public:
	UBTTask_MoveAndShoot(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Category = Node, EditAnywhere, meta=(ClampMin = "0.0"))
	float AcceptableRadius;

	UPROPERTY(Category = Node, EditAnywhere)
	uint32 bAllowStrafe : 1;

	UPROPERTY(Category = Node, EditAnywhere)
	uint32 bObserveBlackboardValue: 1;
    
	//UPROPERTY(Category=Node, EditAnywhere)
	//class TSubclassOf<class UNavigationQueryFilter> FilterClass;

	UPROPERTY(Category=BlackBoard, EditAnywhere)
	FBlackboardKeySelector Key;
	
	//virtual FName GetNodeIconName() const override;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &ownerComp, uint8 *nodeMemory) override;

	
};
