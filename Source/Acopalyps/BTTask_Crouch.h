// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Crouch.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API UBTTask_Crouch : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Crouch();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &ownerComp, uint8 *nodeMemory) override;
};
