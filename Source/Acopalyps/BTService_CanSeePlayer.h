// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson
 *  @author Jonathan Rubensson
 */

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_CanSeePlayer.generated.h"

/**
 * 
 */
UCLASS()
class ACOPALYPS_API UBTService_CanSeePlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_CanSeePlayer();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	

	UPROPERTY()
	class AEnemyAICharacter* EnemyAICharacter;
	
	UPROPERTY()
	APawn* PlayerPawn;

};
