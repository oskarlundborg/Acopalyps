
// Fill out your copyright notice in the Description page of Project Settings.

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
	virtual void Tick(float DeltaSeconds) override;

	/** Method to set if character is in rag-doll-mode in Blackboard
	 * @param true or false depending on if collision preset should be ragdoll or not
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

	/** Calls MoveToActor-func in AIController but passing in set FilterClass to enable prioritizing assigned nav mesh area
	* @param Goal finish move if pawn gets close enough
	* @param bStopOnOverlap	add pawn's radius to AcceptanceRadius
	* @param AcceptanceRadius add pawn's radius to AcceptanceRadius
	* @param bUsePathfinding use navigation data to calculate path (otherwise it will go in straight line)
	* @param bCanStrafe	set focus related flag: bAllowStrafe
	* @param FilterClass navigation filter for pathfinding adjustments. If none specified DefaultNavigationFilterClass will be used
	* @param bAllowPartialPath use incomplete path when goal can't be reached
	 */
	UFUNCTION()
	void MoveToActorFilter(AActor* Goal);
	//virtual void MoveToActor(AActor* Goal, float AcceptanceRadius = 0.f, bool bStopOnOverlap = true, bool bUsePathfinding = true, bool bCanStrafe = false, TSubclassOf<UNavigationQueryFilter> FilterClass = NULL, bool bAllowPartialPath = true);

	/** Array of navigation query filters */
	//TArray<TSubclassOf<UNavigationQueryFilter>>NavigationFiltersClasses;
	
	/** Navigation filter class, set with pseudorandom 
	TSubclassOf<UNavigationQueryFilter> FilterClass;
	*/
	
protected:
	void BeginPlay() override;

private:
	
	APawn* PlayerPawn;

	/** Behavior tree to control behavior */
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;

	
};
