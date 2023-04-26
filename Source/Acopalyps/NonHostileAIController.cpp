// Fill out your copyright notice in the Description page of Project Settings.


#include "NonHostileAIController.h"

// Fill out your copyright notice in the Description page of Project Settings.


#include "NonHostileAIController.h"

#include "AcopalypsCharacter.h"
#include "HealthComponent.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

ANonHostileAIController::ANonHostileAIController()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ANonHostileAIController::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	//PlayerCharacter = Cast<AApocalypsCharacter>(PlayerPawn);
}

void ANonHostileAIController::Initialize()
{
	if(BehaviorTree) RunBehaviorTree(BehaviorTree);
	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), this->GetPawn()->GetActorLocation());
	SetIsRagdoll(false);

	FNavLocation Point0;
	FNavLocation Point1;
	FNavLocation Point2;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	NavSys->GetRandomReachablePointInRadius(GetCharacter()->GetActorLocation(), 1000.f, Point0);
	NavSys->GetRandomReachablePointInRadius(Point0, 1000.f, Point1);
	NavSys->GetRandomReachablePointInRadius(Point1, 1000.f, Point2);
	GetBlackboardComponent()->SetValueAsVector(TEXT("Point0"), Point0.Location);
	GetBlackboardComponent()->SetValueAsVector(TEXT("Point1"), Point1.Location);
	GetBlackboardComponent()->SetValueAsVector(TEXT("Point2"), Point2.Location);
	DrawDebugSphere(GetWorld(), Point0.Location, 15, 6, FColor::Emerald, true);
	DrawDebugSphere(GetWorld(), Point1.Location, 15, 6, FColor::Silver, true);
	DrawDebugSphere(GetWorld(), Point2.Location, 15, 6, FColor::Turquoise, true);
}

void ANonHostileAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANonHostileAIController::SetIsRagdoll(bool val)
{
	GetBlackboardComponent()->SetValueAsBool("IsRagdoll", val);
}
