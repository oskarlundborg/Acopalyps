// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson
	@author Jonathan Rubensson
*/


#include "BTTask_Shoot.h"

#include "AIController.h"
#include "EnemyAICharacter.h"

UBTTask_Shoot::UBTTask_Shoot()
{
	NodeName = TEXT("Shoot");
}

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	Super::ExecuteTask(ownerComp, nodeMemory);

	Cast<AEnemyAICharacter>(ownerComp.GetAIOwner()->GetPawn())->Shoot();
	UE_LOG(LogTemp, Warning, TEXT("Shooting tast"));
	return EBTNodeResult::Succeeded;
}