// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDroneBaseActor.h"

#include "AcopalypsCharacter.h"
#include "HealthComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AEnemyDroneBaseActor::AEnemyDroneBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DroneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Drone Mesh"));
	DroneMesh->SetupAttachment(RootComponent);
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	SphereColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColliderComponent"));
	SphereColliderComponent->InitSphereRadius(20.f);
	SphereColliderComponent->SetupAttachment(RootComponent);
	SphereColliderComponent->SetCollisionProfileName("Drone");
	

	/*
	// Set up the timeline and movement parameters
	MovementCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/MyCurve"));
	TimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("MovementTimeline"));
	TimeLine->AddInterpFloat(MovementCurve, InterpFunction);
	*/
}

// Called when the game starts or when spawned
void AEnemyDroneBaseActor::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCharacter = Cast<AAcopalypsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), PlayerCharacter->GetActorLocation()));

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (PlayerPawn)
		{
			// Use the player location and height to calculate the desired location for the AI character
			PlayerLocation = PlayerPawn->GetActorLocation();
			PlayerHeight = PlayerPawn->GetActorLocation().Z;
		}
	}
	
}

// Called every frame
void AEnemyDroneBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	EngageTargetLocation = GetActorLocation() + PlayerCharacter->GetActorForwardVector() * 20 + PlayerCharacter->GetActorLocation().Y + 20;
	DrawDebugSphere(GetWorld(), EngageTargetLocation, 100, 40, FColor::Red);
	PlayerHeight = PlayerCharacter->GetActorLocation().Z + 20;
	
	//float AngleToPlayer = FMath::DegreesToRadians(45.f); // Set the desired angle from the player here


	// Calculate the location of the AI character relative to the player
	//FVector RelativeLocation = FVector(DistanceToPlayer * FMath::Cos(AngleToPlayer), DistanceToPlayer * FMath::Sin(AngleToPlayer), 0.f);

	// Add the player's location and height to the relative location to get the absolute location for the AI character
	//FVector DesiredLocation = PlayerLocation + RelativeLocation + FVector(0.f, 0.f, PlayerHeight);

	//EngageTargetLocation += DesiredLocation;
	
	GetWorldTimerManager().SetTimer(MoveTimer, this, &AEnemyDroneBaseActor::MoveTowardsEngagedLocation, MoveToEngagedTargetDelay, true);
}

void AEnemyDroneBaseActor::MoveTowardsEngagedLocation()
{
	Direction = (EngageTargetLocation - GetActorLocation()).GetSafeNormal();

	const FVector CurrentLocation = Direction * Speed * GetWorld()->DeltaTimeSeconds;

	SetActorLocation(CurrentLocation);
	/*
	
	if ((EngageTargetLocation - GetActorLocation()).Size() > Movement.Size())
	{
		AddActorWorldOffset(Movement);
	}
	else
	{
		SetActorLocation(EngageTargetLocation);
		GetWorldTimerManager().ClearTimer(MoveTimer);
	}
	*/
}

/*
void AEnemyDroneBaseActor::OnTimelineUpdate(float Value)
{
	// Calculate the new location of the actor based on the timeline value and move it there
	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::Lerp(CurrentLocationLocation, TargetLocation, Value);
	SetActorLocation(NewLocation);
}

FOnTimelineFloat AEnemyDroneBaseActor::InterpFunction()
{
}
*/
void AEnemyDroneBaseActor::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("Overlapping start with: %s"), *OtherActor->GetClass()->GetName());
}

void AEnemyDroneBaseActor::OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Display, TEXT("Overlapping end with: %s"), *OtherActor->GetClass()->GetName());
}




