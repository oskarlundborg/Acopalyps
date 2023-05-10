// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDroneBaseActor.h"

#include "AcopalypsCharacter.h"
#include "FileCache.h"
#include "HealthComponent.h"
#include "VectorTypes.h"
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

	CollisionCheckColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionCheckColliderComponent"));
	CollisionCheckColliderComponent->InitSphereRadius(80.f);
	CollisionCheckColliderComponent->SetupAttachment(RootComponent);
	SphereColliderComponent->SetCollisionProfileName("Drone");
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

	// Start timers to update target location and to check for current state
	StartTimers();
		
	// Binding function to drones general collision check
	CollisionCheckColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyDroneBaseActor::OnCollisionOverlapBegin);
	// Binding function to start and end of overlap with player
	SphereColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyDroneBaseActor::OnAttackOverlapBegin);
	SphereColliderComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemyDroneBaseActor::OnAttackOverlapEnd);
	
	CurrentSpeed = InitialSpeed;
}

void AEnemyDroneBaseActor::StartTimers()
{
	GetWorldTimerManager().SetTimer(UpdateEngagedLocationTimerHandle, this, &AEnemyDroneBaseActor::UpdateEngagedLocationBounds, UpdateEngagedTargetDelay, true, 0.1f);
	GetWorldTimerManager().SetTimer(CheckAttackBoundsTimerHandle, this, &AEnemyDroneBaseActor::CalculateCurrentState, CheckDroneStateDelay, true, 0.1f);
}

// Called every frame
void AEnemyDroneBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTowardsLocation(DeltaTime);
}

/** Moves actor towards a location, depending if drone is attacking or following player*/
void AEnemyDroneBaseActor::MoveTowardsLocation(float DeltaTime)
{
		OnDroneMovement();

		Direction = TargetLocation - GetActorLocation();
		Direction.Normalize();

		// Calculate the distance between the current location and the target location.
		const float Distance = FVector::Distance(TargetLocation, GetActorLocation());

		// Calculate the actual distance the actor can move in this step based on the speed variable.
		float MoveDistance = CurrentSpeed * DeltaTime;
		MoveDistance = FMath::Min(MoveDistance, Distance);
		const FVector NewLocation = GetActorLocation() + Direction * MoveDistance;
		SetActorLocation(NewLocation);
}

/** Updates location from which to start attack*/
FVector AEnemyDroneBaseActor::GetNewEngagedLocation() const
{
	FVector NewLocation;
	int Counter = 0;
	do
	{
		NewLocation = BoundCenterPosition + UKismetMathLibrary::RandomUnitVector() * UKismetMathLibrary::RandomFloatInRange(0, OuterBoundRadius);
		NewLocation = FVector(NewLocation.X, NewLocation.Y, FMath::Clamp(NewLocation.Z, PlayerLocation.Z + MinHeightAbovePlayer, PlayerLocation.Z + MaxHeightAbovePlayer));
		Counter++;
	}
	while (!CheckPathToTargetLocation(NewLocation) && Counter <=10);
	DrawDebugSphere(GetWorld(), NewLocation, 100, 40, FColor::Red, false, 0.5f );
	return NewLocation;
}

/** Updates bounds for location to move toward*/
void AEnemyDroneBaseActor::UpdateEngagedLocationBounds()
{
	PlayerLocation = PlayerCharacter->GetActorLocation();
	PlayerRotation = PlayerCharacter->GetActorRotation();
	
	//BoundCenterPosition = PlayerLocation + PlayerCharacter->GetActorForwardVector() * OuterBoundRadius + FVector(0, 0, MinHeightAbovePlayer);
	BoundCenterPosition = PlayerLocation;
	const FColor SphereColor = FColor::Green;
	DrawDebugSphere(GetWorld(), BoundCenterPosition, InnerBoundRadius, 12, SphereColor, false, 0.5f);
	DrawDebugSphere(GetWorld(), BoundCenterPosition, OuterBoundRadius, 12, FColor::Blue, false, 0.5f);

	TargetLocation = GetNewEngagedLocation();
}

/** Calculates if drone should attack or following player*/
void AEnemyDroneBaseActor::CalculateCurrentState()
{
	if (IsWithinAttackArea())
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Prepare attack")));	
		PrepareForAttack();
	}
}

/** Performs a ray casts, returns true if hit on player is registered */
bool AEnemyDroneBaseActor::CheckPathHitTrace() const
{
	FHitResult HitResult;
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);
	return GetWorld()->SweepSingleByChannel(HitResult, GetActorLocation(), PlayerLocation, FQuat::Identity, ECC_GameTraceChannel4,FCollisionShape::MakeCapsule(Extent), FCollisionQueryParams("DroneSweep", false, this));
}

/** Performs a ray casts, returns true if target location is inside a colliding object. Aka that movement to the point is impossible */
bool AEnemyDroneBaseActor::CheckPathToTargetLocation(FVector NewLocation) const
{
	FHitResult HitResult;
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	return GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), NewLocation, ECollisionChannel::ECC_GameTraceChannel1, Params);
	//return GetWorld()->SweepSingleByChannel(HitResult, GetActorLocation(), NewLocation, ,  ECC_WorldStatic, , FCollisionQueryParams("CheckTargetLocationSweep", false, this ));
	
}

void AEnemyDroneBaseActor::PrepareForAttack()
{
	OnPrepareForAttackEvent();
	bIsPreparingAttack = true;
	CurrentSpeed = 0.1f;
	//logic for when drone is standing still
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyDroneBaseActor::Attack, -1, false, AttackDelay);
}

void AEnemyDroneBaseActor::Attack()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("attack!!!")));

}


/** Checks if drone location is in range to initiate attack*/
bool AEnemyDroneBaseActor::IsWithinAttackArea() const
{
	return (GetActorLocation() - BoundCenterPosition).Length() < InnerBoundRadius;
}

void AEnemyDroneBaseActor::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
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


/*
 * UPDATELOCATIONBOUNDS() löst skit
* // Calculate the boundaries of the box.
	//const FVector BoundCenterPosition = PlayerLocation + PlayerRotation.Vector() * DistanceFromPlayer + FVector(0, 0, HeightAbovePlayer);

	/*
	const FVector BoundCenterPosition = PlayerLocation + PlayerRotation.Vector() * OuterBoundRadius;
	
	BoundTopPosition = BoundCenterPosition + FVector(0.0f, BoundHeight, 0.0f) + FVector(0, 0, HeightAbovePlayer);
	BoundBottomPosition = BoundCenterPosition - FVector(0.0f, BoundHeight, 0.0f);
	BoundLeftEdge = BoundCenterPosition - FVector(BoundWidth, 0.0f, BoundWidth);
	BoundRightEdge = BoundCenterPosition + FVector(BoundWidth, 0.0f, BoundWidth);

	const FVector BoxExtent = FVector(BoundWidth, BoundHeight, BoundWidth);
	const FColor BoxColor = FColor::Green;
	DrawDebugBox(GetWorld(), BoundCenterPosition, BoxExtent, BoxColor, false, 0.5f);

	// Draw a debug sphere around the player character to show the box radius.
	const FColor SphereColor = FColor::Blue;
	DrawDebugSphere(GetWorld(), PlayerLocation, InnerBoundRadius, 12, SphereColor, false, 0.5f);
	*/


/*
 * UPDATE ENGAGED LOCATION INNAN JAG LA TILL BOUNDS
 *
* /*
	PlayerLocation = PlayerCharacter->GetActorLocation();
	PlayerRotation = PlayerCharacter->GetActorRotation();
	
	TargetLocation  = PlayerLocation + PlayerRotation.Vector() * DistanceFromPlayer + FVector(0, 0, HeightAbovePlayer);
	const FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	const FRotator InitialRotation = DirectionToPlayer.ToOrientationRotator();
	SetActorRotation(InitialRotation);
	
*/


