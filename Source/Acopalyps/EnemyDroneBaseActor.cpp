// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDroneBaseActor.h"

#include "AcopalypsCharacter.h"
#include "FileCache.h"
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
	SphereColliderComponent->InitSphereRadius(40.f);
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

	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerCharacter->GetActorLocation()));

	// Calculate the location of the AI character relative to the player, and rotation
	FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	FRotator InitialRotation = DirectionToPlayer.ToOrientationRotator();
	SetActorRotation(InitialRotation);

	GetWorldTimerManager().SetTimer(UpdateEngagedLocationTimerHandle, this, &AEnemyDroneBaseActor::UpdateEngagedLocationBounds, MoveToEngagedTargetDelay, true, 0.1f);

	
}

// Called every frame
void AEnemyDroneBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	Direction = EngageTargetLocation - GetActorLocation();
	Direction.Normalize();

	// Calculate the distance between the current location and the target location.
	float Distance = FVector::Distance(EngageTargetLocation, GetActorLocation());

	// Calculate the actual distance the actor can move in this step based on the speed variable.
	float MoveDistance = InitialSpeed * DeltaTime;
	MoveDistance = FMath::Min(MoveDistance, Distance);
	FVector NewLocation = GetActorLocation() + Direction * MoveDistance;
	SetActorLocation(NewLocation);
	*/
}

void AEnemyDroneBaseActor::MoveTowardsEngagedLocation()
{
	const FVector CurrentLocation = Direction * InitialSpeed * GetWorld()->DeltaTimeSeconds;

	SetActorLocation(CurrentLocation);

}

void AEnemyDroneBaseActor::UpdateEngagedLocation()
{	
	PlayerLocation = PlayerCharacter->GetActorLocation();
	PlayerRotation = PlayerCharacter->GetActorRotation();
	
	EngageTargetLocation  = PlayerLocation + PlayerRotation.Vector() * DistanceFromPlayer + FVector(0, 0, HeightAbovePlayer);
	const FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	const FRotator InitialRotation = DirectionToPlayer.ToOrientationRotator();
	SetActorRotation(InitialRotation);
	DrawDebugSphere(GetWorld(), EngageTargetLocation, 100, 40, FColor::Red, false, 0.5f );
}

void AEnemyDroneBaseActor::UpdateEngagedLocationBounds()
{

	PlayerLocation = PlayerCharacter->GetActorLocation();
	PlayerRotation = PlayerCharacter->GetActorRotation();

	// Calculate the boundaries of the box
	//FVector BoxPosition = PlayerLocation + PlayerRotation.Vector() * DistanceFromPlayer + FVector(0, 0, HeightAbovePlayer);
	const FVector BoundPosition = PlayerLocation + PlayerRotation.Vector() * BoundDistance;
	BoundTopPosition = BoundPosition + FVector(0.0f, BoundHeight, 0.0f);
	BoundBottomPosition = BoundPosition - FVector(0.0f, BoundHeight, 0.0f);
	BoundLeftEdge = BoundPosition - PlayerRotation.RotateVector(FVector(BoundWidth, 0.0f, 0.0f));
	BoundRightEdge = BoundPosition + PlayerRotation.RotateVector(FVector(BoundWidth, 0.0f, 0.0f));

	FVector BoxExtent = FVector(BoundWidth, BoundHeight, BoundWidth);
	BoxExtent = BoxExtent.RotateAngleAxis(PlayerRotation.Yaw, FVector(0.0f, 0.0f, 1.0f));
	const FColor BoxColor = FColor::Green;
	DrawDebugBox(GetWorld(), BoundPosition, BoxExtent, BoxColor, false, 0.5f);
	
	// Calculate the boundaries of the box.
	//const FVector BoundPosition = PlayerLocation + PlayerRotation.Vector() * DistanceFromPlayer + FVector(0, 0, HeightAbovePlayer);

	/*
	const FVector BoundPosition = PlayerLocation + PlayerRotation.Vector() * BoundDistance;
	
	BoundTopPosition = BoundPosition + FVector(0.0f, BoundHeight, 0.0f) + FVector(0, 0, HeightAbovePlayer);
	BoundBottomPosition = BoundPosition - FVector(0.0f, BoundHeight, 0.0f);
	BoundLeftEdge = BoundPosition - FVector(BoundWidth, 0.0f, BoundWidth);
	BoundRightEdge = BoundPosition + FVector(BoundWidth, 0.0f, BoundWidth);

	const FVector BoxExtent = FVector(BoundWidth, BoundHeight, BoundWidth);
	const FColor BoxColor = FColor::Green;
	DrawDebugBox(GetWorld(), BoundPosition, BoxExtent, BoxColor, false, 0.5f);
	*/
	
	// Draw a debug sphere around the player character to show the box radius.
	const FColor SphereColor = FColor::Blue;
	DrawDebugSphere(GetWorld(), PlayerLocation, BoundRadius, 12, SphereColor, false, 0.5f);
}

bool AEnemyDroneBaseActor::IsWithinAttackArea() const
{
	const FVector CurrentLocation = GetActorLocation();
	const float DistanceToLocation = FVector::Distance(PlayerLocation, CurrentLocation);
	if (DistanceToLocation <= BoundRadius &&
		CurrentLocation.X >= BoundLeftEdge.X && CurrentLocation .X <= BoundRightEdge.X &&
		CurrentLocation.Z >= BoundLeftEdge.Z && CurrentLocation .Z <= BoundRightEdge.Z &&
		CurrentLocation.Y >= BoundBottomPosition.Y && CurrentLocation .Y <= BoundTopPosition.Y)
	{
		return true;
	}
	return false;
}

void AEnemyDroneBaseActor::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("Overlapping start with: %s"), *OtherActor->GetClass()->GetName());
}

void AEnemyDroneBaseActor::OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Display, TEXT("Overlapping end with: %s"), *OtherActor->GetClass()->GetName());
}




