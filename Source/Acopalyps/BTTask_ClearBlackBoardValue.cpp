// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ClearBlackBoardValue.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearBlackBoardValue::UBTTask_ClearBlackBoardValue()
{
	NodeName = TEXT("Clear Blackboard Value");
}

EBTNodeResult::Type UBTTask_ClearBlackBoardValue::ExecuteTask(UBehaviorTreeComponent &ownerComp, uint8 *nodeMemory)
{
	Super::ExecuteTask(ownerComp, nodeMemory);
	ownerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	return EBTNodeResult::Succeeded;
	
}