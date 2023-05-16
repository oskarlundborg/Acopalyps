// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveAndShoot.h"

UBTTask_MoveAndShoot::UBTTask_MoveAndShoot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("MoveAndShoot");
}

EBTNodeResult::Type UBTTask_MoveAndShoot::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	return Super::ExecuteTask(ownerComp, nodeMemory);
}
