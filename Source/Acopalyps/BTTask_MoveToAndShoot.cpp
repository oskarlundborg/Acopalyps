// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToAndShoot.h"

#include "EnemyAIController.h"
#include "EnemyDroneBaseActor.h"
#include "MathUtil.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemyAICharacter.h"

UBTTask_MoveToAndShoot::UBTTask_MoveToAndShoot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("MoveAndShoot");

	AcceptableRadius = UKismetMathLibrary::RandomFloatInRange(AcceptableRadiusMin, AcceptableRadiusMax);
	
	// typedefa filters o assigna senFilterClass
}

EBTNodeResult::Type UBTTask_MoveToAndShoot::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type SuperResult = Super::ExecuteTask(OwnerComp, NodeMemory);

	EnemyController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (!EnemyController)
	{
		return EBTNodeResult::Failed;
	}
	
	if (SuperResult != EBTNodeResult::InProgress)
	{
		EnemyController->GetWorldTimerManager().ClearTimer(ShootTimerHandle);
	}

	if (!EnemyController->GetWorldTimerManager().IsTimerActive(ShootTimerHandle))
	{
		ShootingDuration = FMath::RandRange(AcceptableShootDurationMin, AcceptableShootDurationMax);
		EnemyController->GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &UBTTask_MoveToAndShoot::Shoot, ShootingDuration, true, 0.1f); 
	}
	
	return SuperResult;
}

void UBTTask_MoveToAndShoot::Shoot() const
{
	Cast<AEnemyAICharacter>(EnemyController->GetPawn())->Shoot();
}


