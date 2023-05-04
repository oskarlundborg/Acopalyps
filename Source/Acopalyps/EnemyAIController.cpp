// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "AcopalypsCharacter.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProfilingDebugging/CookStats.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	//PlayerCharacter = Cast<AApocalypsCharacter>(PlayerPawn);
}

void AEnemyAIController::SetAim()
{
	FRotator AimRotation;
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	if( GetBlackboardComponent()->GetValueAsBool("CanSeePlayer") && Player != nullptr )
	{
		AimRotation = UKismetMathLibrary::FindLookAtRotation(
				GetCharacter()->GetActorLocation() + GetCharacter()->GetActorRotation().RotateVector(FVector(-10, 0, 8)),
				Player->GetActorLocation()
				);
	}
	GetCharacter()->SetActorRotation(AimRotation);
}

void AEnemyAIController::Initialize()
{
	if(BehaviorTree) RunBehaviorTree(BehaviorTree);
	UE_LOG(LogTemp, Warning, TEXT("BehaviourTReetorun %s"), *BehaviorTree->GetName());
	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), this->GetPawn()->GetActorLocation());
	GetBlackboardComponent()->SetValueAsObject(TEXT("Player"), UGameplayStatics::GetPlayerCharacter(this, 0));
	SetIsRagdoll(false);

	/*
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem)
	{
		APawn* MyPawn = GetPawn();
		const ANavigationData* NavData = NavSystem->GetNavDataForProps(MyPawn->GetNavAgentPropertiesRef());
	}
	*/
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AEnemyAIController::SetIsRagdoll(bool val)
{
	GetBlackboardComponent()->SetValueAsBool("IsRagdoll", val);
}



bool AEnemyAIController::HitTraceAtPLayerSuccess()
{

	FHitResult HitResult;
	FVector Origin, Extent;
	PlayerPawn->GetActorBounds(true, Origin, Extent);

	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorLocation() + GetCharacter()->GetCharacterMovement()->Velocity.GetSafeNormal() * (GetCharacter()->GetCharacterMovement()->Velocity.Size()), FQuat::Identity, ECC_Pawn,FCollisionShape::MakeCapsule(Extent), FCollisionQueryParams("BoxSweep", false, this));

	if (bHit)
	{
		AAcopalypsCharacter* HitCharacter = Cast<AAcopalypsCharacter>(HitResult.GetActor());
		if(HitCharacter)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}
