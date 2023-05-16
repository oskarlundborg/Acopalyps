// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToAndShoot.h"

#include "EnemyDroneBaseActor.h"
#include "MathUtil.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTask_MoveToAndShoot::UBTTask_MoveToAndShoot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("MoveAndShoot");

	AcceptableRadius = UKismetMathLibrary::RandomFloatInRange(AcceptableRadiusMin, AcceptableRadiusMax);
	
	// typedefa filters o assigna senFilterClass
}

EBTNodeResult::Type UBTTask_MoveToAndShoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type SuperResult =Super::ExecuteTask(OwnerComp, NodeMemory);

	EnemyActor = Cast<AActor>(&OwnerComp);

	if (!EnemyActor) { return EBTNodeResult::Failed; }
	
	if (SuperResult != EBTNodeResult::InProgress)
	{
		EnemyActor->GetWorldTimerManager().ClearTimer(ShootTimerHandle);
	}

	if (!EnemyActor->GetWorldTimerManager().IsTimerActive(ShootTimerHandle))
	{
		ShootingDuration = FMath::RandRange(AcceptableShootDurationMin, AcceptableShootDurationMax);
		EnemyActor->GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &UBTTask_MoveToAndShoot::Shoot, ShootingDuration, true, 0.1f); 
	}
	
	return SuperResult;
}

void UBTTask_MoveToAndShoot::Shoot()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Shooting from new task")));
}

/*
void UBTTask_MoveToAndShoot::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}
*/
