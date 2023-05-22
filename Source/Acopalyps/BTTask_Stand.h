// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson
	@author Jonathan Rubensson
*/

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Stand.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API UBTTask_Stand : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Stand();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &ownerComp, uint8 *nodeMemory) override;
};
