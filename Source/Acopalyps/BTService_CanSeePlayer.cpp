// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CanSeePlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTService_CanSeePlayer::UBTService_CanSeePlayer()
{
	NodeName = TEXT("Can see player?");
}

void UBTService_CanSeePlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), false);
		return;
	}
	if (OwnerComp.GetAIOwner() == nullptr) return;

	if (OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn, OwnerComp.GetAIOwner()->GetCharacter()->GetActorLocation() + FVector(0, 0, 100)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Sees player"), true);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), false);
	}
	/*
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("CanSeePlayer"), SeesPlayer);
	}
	/*
	//APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	//if(!PlayerController)
	//{
	//	OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("CanSeePlayer"), false);
	//	return;
	//}

	bool SeesPlayer = OwnerComp.GetAIOwner()->LineOfSightTo(PlayerController);
	if (SeesPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("YAy sees blayer"));
		
	}
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("CanSeePlayer"), SeesPlayer);
	*/
}
