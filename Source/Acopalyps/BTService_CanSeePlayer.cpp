// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson
 *  @author Jonathan Rubensson
 */


#include "BTService_CanSeePlayer.h"

#include "AcopalypsCharacter.h"
#include "AIController.h"
#include "CoverPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "EnemyAICharacter.h"
#include "EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_CanSeePlayer::UBTService_CanSeePlayer()
{
	NodeName = TEXT("Can see player?");
}

void UBTService_CanSeePlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), false);
		return;
	}
	if (OwnerComp.GetAIOwner() == nullptr) return;

	EnemyAICharacter = Cast<AEnemyAICharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (EnemyAICharacter == nullptr) return;
	
	if (OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn)) 
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);
	}
	else
	{
		if (EnemyAICharacter->GetController())
		{
			AEnemyAIController* OwnerController = Cast<AEnemyAIController>(EnemyAICharacter->GetController());
			if (OwnerController){
				ACoverPoint* Cover = Cast<ACoverPoint>(OwnerController->GetBlackboardComponent()->GetValueAsObject("Cover"));
				if(Cover)
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), Cover->HasLineOfSightToPlayer());
				}
			}
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), false);
		}
	}
}
