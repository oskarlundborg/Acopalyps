// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindCover.h"

#include "CoverPoint.h"
#include "Kismet/GameplayStatics.h"

UBTService_FindCover::UBTService_FindCover()
{
	NodeName = TEXT("Update cover chosen");
}

void UBTService_FindCover::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (playerPawn == nullptr) return;

	if (OwnerComp.GetAIOwner() == nullptr) return;

	
}

bool UBTService_FindCover::IsCoverValid(ACoverPoint* Cover)
{
	return Cover->GetActorForwardVector().Dot(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation() - Cover->GetActorLocation()) > 0;
}
