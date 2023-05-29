// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */

#include "EnemyAIController.h"
#include "AcopalypsCharacter.h"
#include "EnemyAICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/CrowdFollowingComponent.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// kanske göra en mer random metod? Coin flip??
	/*
	const int RandomFilterIndex = FMath::RandRange(0, SubclassQueryFilters.Num()-1);
	EnemyFilterClass = SubclassQueryFilters[RandomFilterIndex];

	DefaultNavigationFilterClass = EnemyFilterClass;

	AEnemyAICharacter* MyCharacter = Cast<AEnemyAICharacter>(GetCharacter());
	if (MyCharacter)
	{
		MyCharacter->SetFilter(EnemyFilterClass);
	}
	*/
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
	GetCharacter()->SetActorRotation(FRotator(AimRotation.Pitch, AimRotation.Yaw, 0));
}

void AEnemyAIController::Initialize()
{
	if(BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
	UE_LOG(LogTemp, Warning, TEXT("BehaviourTReetorun %s"), *BehaviorTree->GetName());
	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), this->GetPawn()->GetActorLocation());
	GetBlackboardComponent()->SetValueAsObject(TEXT("Player"), UGameplayStatics::GetPlayerCharacter(this, 0));
	SetIsRagdoll(false);
}


void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AEnemyAIController::SetIsRagdoll(bool val)
{
	GetBlackboardComponent()->SetValueAsBool("IsRagdoll", val);
}

bool AEnemyAIController::HitTraceAtPLayerSuccess() const
{

	FHitResult HitResult;
	FVector Origin, Extent;
	PlayerPawn->GetActorBounds(true, Origin, Extent);

	const bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetCharacter()->GetActorLocation(),
		GetCharacter()->GetActorLocation() + GetCharacter()->GetCharacterMovement()->Velocity.GetSafeNormal() * (GetCharacter()->GetCharacterMovement()->Velocity.Size()),
		FQuat::Identity,
		ECC_Pawn,FCollisionShape::MakeCapsule(Extent),
		FCollisionQueryParams("BoxSweep", false, this)
		);

	if (bHit && Cast<AAcopalypsCharacter>(HitResult.GetActor()))
	{
		return true;
	}
	return false;
}


