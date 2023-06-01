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
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Tasks/AITask_MoveTo.h"

UBTTask_MoveToAndShoot::UBTTask_MoveToAndShoot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("MoveAndShoot");
	
	AcceptableRadius = UKismetMathLibrary::RandomFloatInRange(AcceptableRadiusMin, AcceptableRadiusMax);
}

/** Overriding MoveTos ExecuteTask to allow pseudorandom shooting between given interval*/
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

/** Overriding MoveTos PerformMoveTask to account for navqueryfilters*/
EBTNodeResult::Type UBTTask_MoveToAndShoot::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	FBTMoveToTaskMemory* MyMemory = CastInstanceNodeMemory<FBTMoveToTaskMemory>(NodeMemory);
	AAIController* MyController = OwnerComp.GetAIOwner();

	const float AcceptableRadiusResolved = UKismetMathLibrary::RandomFloatInRange(AcceptableRadiusMin, AcceptableRadiusMax);
	const bool AllowPartialPath = false;

	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;
	if (MyController && MyBlackboard && AcceptableRadiusResolved > 0.0f) {
		FAIMoveRequest MoveReq;
		MoveReq.SetNavigationFilter(MyController->GetDefaultNavigationFilterClass());
		MoveReq.SetAllowPartialPath(AllowPartialPath);
		MoveReq.SetAcceptanceRadius(AcceptableRadiusResolved);
		MoveReq.SetCanStrafe(bAllowStrafe);
		MoveReq.SetReachTestIncludesAgentRadius(bReachTestIncludesAgentRadius);
		MoveReq.SetReachTestIncludesGoalRadius(bReachTestIncludesGoalRadius);
		MoveReq.SetProjectGoalLocation(bProjectGoalLocation);
		MoveReq.SetUsePathfinding(bUsePathfinding);

		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass()) {
			UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
			AActor* TargetActor = Cast<AActor>(KeyValue);
			if (TargetActor) {
				if (bTrackMovingGoal) {
					MoveReq.SetGoalActor(TargetActor);
				}
				else {
					MoveReq.SetGoalLocation(TargetActor->GetActorLocation());
				}
			}
			else {
				// UE_VLOG(MyController, LogBehaviorTree, Warning, TEXT("UBTTask_MoveTo::ExecuteTask tried to go to actor while BB %s entry was empty"), *BlackboardKey.SelectedKeyName.ToString());
			}
		}
		else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass()) {
			const FVector TargetLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
			MoveReq.SetGoalLocation(TargetLocation);

			MyMemory->PreviousGoalLocation = TargetLocation;
		}

		if (MoveReq.IsValid()) {
			if (GET_AI_CONFIG_VAR(bEnableBTAITasks)) {
				UAITask_MoveTo* MoveTask = MyMemory->Task.Get();
				const bool bReuseExistingTask = (MoveTask != nullptr);

				MoveTask = PrepareMoveTask(OwnerComp, MoveTask, MoveReq);
				if (MoveTask) {
					MyMemory->bObserverCanFinishTask = false;

					if (bReuseExistingTask) {
						if (MoveTask->IsActive()) {
							// UE_VLOG(MyController, LogBehaviorTree, Verbose, TEXT("\'%s\' reusing AITask %s"), *GetNodeName(), *MoveTask->GetName());
							MoveTask->ConditionalPerformMove();
						}
						else {
							// UE_VLOG(MyController, LogBehaviorTree, Verbose, TEXT("\'%s\' reusing AITask %s, but task is not active - handing over move performing to task mechanics"), *GetNodeName(), *MoveTask->GetName());
						}
					}
					else {
						MyMemory->Task = MoveTask;
						// UE_VLOG(MyController, LogBehaviorTree, Verbose, TEXT("\'%s\' task implementing move with task %s"), *GetNodeName(), *MoveTask->GetName());
						MoveTask->ReadyForActivation();
					}

					MyMemory->bObserverCanFinishTask = true;

					//ensure(MoveTask->WasMoveSuccessful() == true);

					NodeResult = (MoveTask->GetState() != EGameplayTaskState::Finished)
						             ? EBTNodeResult::InProgress
						             : MoveTask->WasMoveSuccessful()
						             ? EBTNodeResult::Succeeded
						             : EBTNodeResult::Failed;
				}
			}
			else {
				FPathFollowingRequestResult RequestResult = MyController->MoveTo(MoveReq);
				if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful) {
					MyMemory->MoveRequestID = RequestResult.MoveId;
					WaitForMessage(OwnerComp, UBrainComponent::AIMessage_MoveFinished, RequestResult.MoveId);
					WaitForMessage(OwnerComp, UBrainComponent::AIMessage_RepathFailed);

					NodeResult = EBTNodeResult::InProgress;
				}
				else if (RequestResult.Code == EPathFollowingRequestResult::AlreadyAtGoal) {
					NodeResult = EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return NodeResult;
}

/** Shoots at player for a random amount of time before continuing running*/
void UBTTask_MoveToAndShoot::Shoot() 
{
	if( auto Player = UGameplayStatics::GetPlayerCharacter(this, 0) )
	{
		if (EnemyController->IsValidLowLevel() && EnemyController->LineOfSightTo(Player))
		{
			Cast<AEnemyAICharacter>(EnemyController->GetPawn())->Shoot();
		}
		EnemyController->GetWorldTimerManager().ClearTimer(ShootTimerHandle);
	}
}


