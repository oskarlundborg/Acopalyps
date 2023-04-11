// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "AcopalypsCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree Component"));
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	//PlayerCharacter = Cast<AApocalypsCharacter>(PlayerPawn);
	
	if(BehaviorTree) RunBehaviorTree(BehaviorTree);
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerStartLocation"), PlayerPawn->GetActorLocation());
	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), this->GetPawn()->GetActorLocation());
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

