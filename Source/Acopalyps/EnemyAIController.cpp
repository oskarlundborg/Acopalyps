// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "AcopalypsCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
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
	if( GetBlackboardComponent()->GetValueAsBool("CanSeePlayer"))
	{
		AimRotation = UKismetMathLibrary::FindLookAtRotation(
				GetCharacter()->GetActorLocation() + GetCharacter()->GetActorRotation().RotateVector(FVector(-10, 0, 8)),
				UGameplayStatics::GetPlayerCharacter(this, 0)->GetActorLocation()
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
}


void AEnemyAIController::SetAim()
{
	FRotator AimRotation;
	if( GetBlackboardComponent()->GetValueAsBool("CanSeePlayer"))
	{
		AimRotation = UKismetMathLibrary::FindLookAtRotation(
				GetCharacter()->GetActorLocation() + GetCharacter()->GetActorRotation().RotateVector(FVector(-10, 0, 8)),
				UGameplayStatics::GetPlayerCharacter(this, 0)->GetActorLocation()
				);
	}
	GetCharacter()->SetActorRotation(AimRotation);
}



void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AEnemyAIController::SetIsRagdoll(bool val)
{
	GetBlackboardComponent()->SetValueAsBool("IsRagdoll", val);
}
