// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */

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
	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn == nullptr) return;

	if (OwnerComp.GetAIOwner() == nullptr) return;

	EnemyAICharacter = Cast<AEnemyAICharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (EnemyAICharacter == nullptr) return;
	/**
	If the character already has a cover we check if it is still valid and not too far away to be usable. If it isn't
	we clear the cover of it's pointer to this character as well as the blackboard value and set the last visited
	character to the active character. Tracking the last visited character lets us make sure characters don't use the
	same cover point twice in a row, this to add some dynamism to their behavior.
	*/
	if(ACoverPoint* CurrentCover = Cast<ACoverPoint>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Cover")))
	{
		if(IsCoverValid(CurrentCover) && FVector::Distance(EnemyAICharacter->GetActorLocation(), CurrentCover->GetActorLocation()) < FVector::Distance(PlayerPawn->GetActorLocation(), EnemyAICharacter->GetActorLocation()) / 3)
		{
			return;
		}
		CurrentCover->bIsOccupied = false;
		CurrentCover->LastVisitedCharacter = EnemyAICharacter;
		OwnerComp.GetBlackboardComponent()->ClearValue("Cover");
	}

	/**
	The rest of the code in this function does a sphere overlap, centered between the enemy character and the player
	character. The radius reaches both characters with an added margin of three meters. The overlap is generated and
	the first valid point of cover that was not last visited by this character.
	*/
	const FVector MyLocation = OwnerComp.GetAIOwner()->GetCharacter()->GetActorLocation();
	const double HalfDistanceToPlayer = (PlayerPawn->GetActorLocation() - MyLocation).Length();
	const FVector MidwayPoint = MyLocation + (PlayerPawn->GetActorLocation() - MyLocation) / 2;
	const FCollisionShape CheckSphereShape = FCollisionShape::MakeSphere(HalfDistanceToPlayer + 300);
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

//Helper function that checks if a CoverPoint is facing towards the players current position
bool UBTService_FindCover::IsCoverValid(const ACoverPoint* Cover) const
{
	/**Checking if the dot product between the cover's forward vector and the vector from the cover to the player is
	positive tells us if the player is in front of the cover or not */
	return Cover->GetActorForwardVector().Dot(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation() - Cover->GetActorLocation()) > 0;
}
