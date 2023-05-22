// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson
	@author Jonathan Rubensson
*/


#include "BTTask_Stand.h"

#include "AIController.h"
#include "GameFramework/Character.h"

UBTTask_Stand::UBTTask_Stand()
{
	NodeName = TEXT("Stand");
}

EBTNodeResult::Type UBTTask_Stand::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if(OwnerComp.GetAIOwner() == nullptr) return EBTNodeResult::Failed;

	OwnerComp.GetAIOwner()->GetCharacter()->UnCrouch();

	return EBTNodeResult::Succeeded;
}