// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson
 *  @author Jonathan Rubensson
 */


#include "BTTask_Crouch.h"

#include "AIController.h"
#include "GameFramework/Character.h"

UBTTask_Crouch::UBTTask_Crouch()
{
	NodeName = TEXT("Crouch");
}

EBTNodeResult::Type UBTTask_Crouch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if(OwnerComp.GetAIOwner() == nullptr) return EBTNodeResult::Failed;

	OwnerComp.GetAIOwner()->GetCharacter()->Crouch();

	return EBTNodeResult::Succeeded;
}
