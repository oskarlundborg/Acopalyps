// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindCover.h"

#include "AIController.h"
#include "CoverPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "EnemyAICharacter.h"
#include "Kismet/GameplayStatics.h"

UBTService_FindCover::UBTService_FindCover()
{
	NodeName = TEXT("Update cover chosen");
}

void UBTService_FindCover::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn == nullptr) return;

	if (OwnerComp.GetAIOwner() == nullptr) return;

	EnemyAICharacter = Cast<AEnemyAICharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (EnemyAICharacter == nullptr) return;

	ACoverPoint* CurrentCover = Cast<ACoverPoint>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Cover"));
	if(CurrentCover)
	{
		if(IsCoverValid(CurrentCover) && FVector::Distance(EnemyAICharacter->GetActorLocation(), CurrentCover->GetActorLocation()) < FVector::Distance(PlayerPawn->GetActorLocation(), EnemyAICharacter->GetActorLocation()) / 3)
			return;
		CurrentCover->bIsOccupied = false;
		CurrentCover->LastVisitedCharacter = nullptr;
		OwnerComp.GetBlackboardComponent()->ClearValue("Cover");
	}
	FVector MyLocation = OwnerComp.GetAIOwner()->GetCharacter()->GetActorLocation();
	double HalfDistanceToPlayer = (PlayerPawn->GetActorLocation() - MyLocation).Length();
	FVector MidwayPoint = MyLocation + (PlayerPawn->GetActorLocation() - MyLocation) / 2;
	FCollisionShape CheckSphereShape = FCollisionShape::MakeSphere(HalfDistanceToPlayer + 300);
	FCollisionObjectQueryParams Params = FCollisionObjectQueryParams();
	Params.AddObjectTypesToQuery(ECC_GameTraceChannel8);
	TArray<FOverlapResult> OverlapResults;
	//DrawDebugSphere(GetWorld(), MidwayPoint, HalfDistanceToPlayer + 300, 24, FColor::Turquoise, false, .5f);
	bool bOverlaps = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		MidwayPoint,
		FQuat::Identity,
		Params,
		CheckSphereShape);
	if(bOverlaps)
	{
		//UE_LOG(LogTemp, Warning, TEXT("overlaps: %i"), OverlapResults.Num());

		for(FOverlapResult Overlap : OverlapResults)
		{
			ACoverPoint* CoverPoint = Cast<ACoverPoint>(Overlap.GetActor());
			if(CoverPoint && !CoverPoint->bIsOccupied && IsCoverValid(CoverPoint) && CoverPoint->LastVisitedCharacter != EnemyAICharacter)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject("Cover", CoverPoint);
				UE_LOG(LogTemp, Warning, TEXT("Cover is set"));
				CoverPoint->bIsOccupied = true;
				CoverPoint->LastVisitedCharacter = EnemyAICharacter;
				return;
			}
		}
	}
}

bool UBTService_FindCover::IsCoverValid(ACoverPoint* Cover)
{
	return Cover->GetActorForwardVector().Dot(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation() - Cover->GetActorLocation()) > 0;
}
