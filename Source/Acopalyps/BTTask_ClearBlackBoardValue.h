// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ClearBlackBoardValue.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API UBTTask_ClearBlackBoardValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ClearBlackBoardValue();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &ownerComp, uint8 *nodeMemory) override;
};
