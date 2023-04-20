// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FindCover.generated.h"

class ACoverPoint;
/**
 * 
 */
UCLASS()
class ACOPALYPS_API UBTService_FindCover : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_FindCover();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsCoverValid(ACoverPoint* Cover);
};
