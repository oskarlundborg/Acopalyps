// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */

#include "BTTask_MoveToAndShoot.h"

#include "EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyDroneBaseActor.h"
#include "MathUtil.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemyAICharacter.h"
#include "Kismet/GameplayStatics.h"

UBTTask_MoveToAndShoot::UBTTask_MoveToAndShoot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("MoveAndShoot");
	
	AcceptableRadius = UKismetMathLibrary::RandomFloatInRange(AcceptableRadiusMin, AcceptableRadiusMax);

	
}

EBTNodeResult::Type UBTTask_MoveToAndShoot::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type SuperResult = Super::ExecuteTask(OwnerComp, NodeMemory);

	EnemyController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (!EnemyController)
	{
		return EBTNodeResult::Failed;
	}

	UCharacterMovementComponent* PlayerMovementComponent = Cast<UCharacterMovementComponent>(OwnerComp.GetAIOwner()->GetPawn()->GetMovementComponent());
	if (PlayerMovementComponent)
	{
		PlayerMovementComponent->MaxWalkSpeed = UKismetMathLibrary::RandomFloatInRange(MinSpeedInterval, MaxSpeedInterval);
	}
	

	if (!EnemyController->GetWorldTimerManager().IsTimerActive(ShootTimerHandle))
	{
		ShootingDuration = FMath::RandRange(AcceptableShootDurationMin, AcceptableShootDurationMax);
		EnemyController->GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &UBTTask_MoveToAndShoot::Shoot, 0.1f, false, ShootingDuration); 
	}

	if (SuperResult != EBTNodeResult::InProgress)
	{
		EnemyController->GetWorldTimerManager().ClearTimer(ShootTimerHandle);
	}
	
	return SuperResult;
}

void UBTTask_MoveToAndShoot::Shoot() 
{
	if (EnemyController && EnemyController->LineOfSightTo(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Cast<AEnemyAICharacter>(EnemyController->GetPawn())->Shoot();
	}
	EnemyController->GetWorldTimerManager().ClearTimer(ShootTimerHandle);

}


