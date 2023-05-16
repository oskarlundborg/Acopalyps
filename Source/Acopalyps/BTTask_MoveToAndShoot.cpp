// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToAndShoot.h"

#include "MathUtil.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_MoveToAndShoot::UBTTask_MoveToAndShoot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("MoveAndShoot");

	AcceptableRadius = UKismetMathLibrary::RandomFloatInRange(AcceptableRadiusMin, AcceptableRadiusMax);

	// typedefa filters o assigna senFilterClass 
}

EBTNodeResult::Type UBTTask_MoveToAndShoot::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	return Super::ExecuteTask(ownerComp, nodeMemory);
}
