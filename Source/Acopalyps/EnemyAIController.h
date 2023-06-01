
// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "EnemyAIController.generated.h"

class UNavigationQueryFilter;
/**
 * 
 */
UCLASS()
class ACOPALYPS_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	

public:

	AEnemyAIController(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;

	/** Method to set if character is in rag-doll-mode in Blackboard
	 * @param val true or false depending on if collision preset should be ragdoll or not
	 */
	void SetIsRagdoll(bool val);

	/** Runs Behavior tree if not null, and sets keys in Blackboard */
	void Initialize();

	/** Sets actor rotation towards player before shooting, enables shooting player when standing on lower/higher ground */
	void SetAim();

	/** Single sweeps towards player 
	 *@return whether hitresult was player character
	 */
	bool HitTraceAtPLayerSuccess() const;

	/** ========= Filters currently not in use, but can be used for crowd avoidance*/
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UNavigationQueryFilter>> SubclassQueryFilters;
	
	TSubclassOf<UNavigationQueryFilter> EnemyFilterClass;

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

private:
	
	APawn* PlayerPawn;

	/** Behavior tree to control behavior */
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;
	
};
