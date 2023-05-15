// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDroneBaseActor.h"

#include "AcopalypsCharacter.h"
#include "AcopalypsPrototypeGameModeBase.h"
#include "FileCache.h"
#include "HealthComponent.h"
#include "VectorTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AEnemyDroneBaseActor::AEnemyDroneBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DroneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Drone Mesh"));
	RootComponent = DroneMesh;
	
	SphereColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColliderComponent"));
	SphereColliderComponent->InitSphereRadius(60.f);
	SphereColliderComponent->SetCollisionProfileName("Drone");
	SphereColliderComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, TEXT("NAME_None"));
	SphereColliderComponent->SetupAttachment(RootComponent);
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

// Called when the game starts or when spawned
void AEnemyDroneBaseActor::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCharacter = Cast<AAcopalypsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerCharacter->GetActorLocation()));

	// Calculate the location of the AI character relative to the player, and rotation
	const FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	const FRotator InitialRotation = DirectionToPlayer.ToOrientationRotator();
	SetActorRotation(InitialRotation);

	// Start timers to update target location and to check for current state
	StartTimers();

	// Save home location
	//GetWorldTimerManager().SetTimer(SaveHomeLocationTimerHandle, this, &AEnemyDroneBaseActor::SaveHomeLocation, 0.1f, false, SaveHomeDelay);
	
	// Binding function to start of overlap with player
	SphereColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyDroneBaseActor::OnOverlapBegin);
	
	CurrentSpeed = InitialSpeed;

	GenerateNewRelativePosition();
	
	bIdle = true;
	bHasMetPlayer = false;
}

void AEnemyDroneBaseActor::StartTimers()
{
	GetWorldTimerManager().SetTimer(UpdateCurrentObjectiveTimerHandle, this, &AEnemyDroneBaseActor::UpdateCurrentObjective, UpdateCurrentObjectiveDelay, true, 0.1f); // 0.2f;
	//GetWorldTimerManager().SetTimer(CheckPlayerDistanceTimerHandle, this, &AEnemyDroneBaseActor::HasLostPlayer, CheckPlayerDistanceDelay, true, 0.1f); // 1.f;
	GetWorldTimerManager().SetTimer(CheckCollisionTimerHandle, this, &AEnemyDroneBaseActor::AdjustMovementForCollision, CheckCollisionDelay, true, 0.1f); // 0.2f;
	//GetWorldTimerManager().SetTimer(UpdateEngagedLocationTimerHandle, this, &AEnemyDroneBaseActor::CalculateEngagedLocation, UpdateEngagedLocationDelay, true, 0.1f); // 0.2f;
	GetWorldTimerManager().SetTimer(CheckAttackBoundsTimerHandle, this, &AEnemyDroneBaseActor::CheckAttackPotential, CheckAttackPotentialDelay, true, 0.1f); //0.2f;
}

void AEnemyDroneBaseActor::StopTimers()
{
	GetWorldTimerManager().ClearTimer(UpdateEngagedLocationTimerHandle);
	GetWorldTimerManager().ClearTimer(CheckAttackBoundsTimerHandle);
}

// Called every frame
void AEnemyDroneBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlayerLocation = PlayerCharacter->GetActorLocation();
	PlayerRotation = PlayerCharacter->GetActorRotation();

	MoveTowardsLocation(DeltaTime);
}

/** Moves actor towards a location */
void AEnemyDroneBaseActor::MoveTowardsLocation(float DeltaTime)
{
	OnDroneMovement();
	
	//Direction = EngagedLocation - GetActorLocation();
	Direction = CurrentTargetLocation - GetActorLocation();
	Direction.Normalize();

	// Calculate the distance between the current location and the target location
	const float Distance = FVector::Distance(CurrentTargetLocation, GetActorLocation());

	// Calculate distance the actor can move in this step based on the speed variable
	float MoveDistance = CurrentSpeed * DeltaTime;
	MoveDistance = FMath::Min(MoveDistance, Distance);
	const FVector NewLocation = GetActorLocation() + Direction * MoveDistance;
	SetActorLocation(NewLocation);
}

void AEnemyDroneBaseActor::UpdateCurrentObjective()
{
	if (bIdle && !bAttack)
	{
		CalculateEngagedLocation();
		CurrentTargetLocation = EngagedLocation;
	}
	else if (!bIdle && bAttack)
	{
		CurrentTargetLocation = PlayerLocation;
	}
	else if (!bIdle && !bAttack)
	{
		CalculateEngagedLocation();
		CurrentTargetLocation = EngagedLocation;
	}
}

/*
void AEnemyDroneBaseActor::SaveHomeLocation()
{
	HomeLocation = GetActorLocation();
}
*/

/** Updates location from which to start attack*/
void AEnemyDroneBaseActor::CalculateEngagedLocation() 
{
	BoundCenterPosition = PlayerLocation;
	if (DebugAssist)
	{
		DrawDebugSphere(GetWorld(), PlayerLocation, InnerBoundRadius, 30, FColor::Blue, true,1.f);
		DrawDebugSphere(GetWorld(), PlayerLocation, OuterBoundRadius, 30, FColor::Green, true,1.f);

	}
	FVector NewLocation;
	int Counter = 0;
	do
	{
		NewLocation = BoundCenterPosition + RelativePositionToPLayer;
		NewLocation = FVector(NewLocation.X, NewLocation.Y, FMath::Clamp(NewLocation.Z, PlayerLocation.Z + MinHeightAbovePlayer, PlayerLocation.Z + MaxHeightAbovePlayer));
		Counter++;
	}
	while (IsTargetLocationValid(NewLocation) && Counter <=10);

	/*
	// kollar mot dit man vill om det finns nåt ivägen, och adjustar utefter det SKA INTE VARA HÄR
	if (CollisionOnPathToTarget(NewLocation))
	{
		NewLocation = GetAdjustedLocation(NewLocation);
	}
	// hit ska saker läggas nån annans stans
	*/
	EngagedLocation = NewLocation;
}

void AEnemyDroneBaseActor::GenerateNewRelativePosition()
{
	//KANSKE MÅSTE GÖRAS GENERALISERBAR. 
	RelativePositionToPLayer = FVector(UKismetMathLibrary::RandomUnitVector()) * UKismetMathLibrary::RandomFloatInRange(InnerBoundRadius, OuterBoundRadius);
}

/** Calculates if drone should attack or following player*/
void AEnemyDroneBaseActor::CheckAttackPotential()
{
	if (IsWithinAttackArea() && IsTargetLocationValid(EngagedLocation) && bIdle && !bAttack)
	{
		PrepareForAttack();
	}
}

void AEnemyDroneBaseActor::AdjustMovementForCollision()
{
	if (CollisionOnPathToTarget(CurrentTargetLocation))
	{
		CurrentTargetLocation = GetAdjustedLocation(CurrentTargetLocation );
	}
}

/** Performs a ray casts, returns a location to move towards if colliding object found between target location and current location to avoid collision*/
bool AEnemyDroneBaseActor::CollisionOnPathToTarget(FVector NewLocation)
{
	FHitResult HitResult;
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);
	
	// Define the object types to trace against
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel3)); // Enemy

	TArray<AActor*> IgnoreActors = TArray<AActor*>();
	IgnoreActors.Add(this);
	
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), GetActorLocation(),
		NewLocation, DroneRadius , ObjectTypes, true, IgnoreActors, EDrawDebugTrace::Persistent,
		HitResult, true, FColor::Green, FLinearColor::Red,  1.f)
)
	{
		SweepHitResult = HitResult;
		return true;
	}
	return false;
}

FVector AEnemyDroneBaseActor::GetAdjustedLocation(FVector GoalLocation) 
{
	// Get the hit point and surface normal
	const FVector HitPoint = SweepHitResult.ImpactPoint;

	// Calculate a location around collision
	const FVector SurfaceNormal = SweepHitResult.ImpactNormal;
	FVector AdjustedLocation = HitPoint +  SurfaceNormal * CollisionAvoidanceOffset;

	// Distance vector between drone and player location aka hypotenusan
	const FVector DroneToGoal = ((PlayerLocation + RelativePositionToPLayer) - GetActorLocation()).GetSafeNormal();

	// Projection of DroneToPlayer on SurfaceNormal
	const double ProjectionOnSurfNorm = -SurfaceNormal.Dot(DroneToGoal);

	if (ProjectionOnSurfNorm > 0.93)
	{
		GenerateNewRelativePosition();
	}
	
	// Magnitude of distance vector aka långa katetern
	const double DistanceMagnitude = DroneToGoal.Size();
	
	const FVector AdjustedDirection = DroneToGoal + SurfaceNormal * DistanceMagnitude * ProjectionOnSurfNorm;
	AdjustedLocation += AdjustedDirection;
	if (DebugAssist)
	{
		DrawDebugSphere(GetWorld(), HitPoint, 30.f, 30, FColor::Black, true,1.f);
		DrawDebugSphere(GetWorld(), AdjustedLocation, 30.f, 30, FColor::Orange, true,1.f);
	}
	return AdjustedLocation;
}

/** Performs a ray casts, returns true if target location is inside a colliding object. Aka that movement to the point is impossible */
bool AEnemyDroneBaseActor::IsTargetLocationValid(FVector NewLocation) const
{
	FHitResult HitResult;
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	return !GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), NewLocation, ECC_GameTraceChannel1, Params);
}

/** Runs when drone is in range of player to start attack */
void AEnemyDroneBaseActor::PrepareForAttack()
{
	OnPrepareForAttackEvent();
	bIdle = false;
	CurrentSpeed = 0.1f;
	//logic for when drone is standing still
	//StopTimers();
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyDroneBaseActor::Attack, 0.1f, false, AttackDelay);
}

/** Runs when PrepareForAttack is done, starts drones unique attack pattern*/
void AEnemyDroneBaseActor::Attack()
{
	OnAttackEvent();
	CurrentSpeed = AttackSpeed;
	// = PlayerLocation;
	bAttack = true;
	GetWorldTimerManager().SetTimer(RetreatTimerHandle, this, &AEnemyDroneBaseActor::Retreat, 0.1f, false, RetreatDelay);
}

/** Runs after attack, to get a new target position */
void AEnemyDroneBaseActor::Retreat()
{
	OnRetreatEvent();
	CurrentSpeed = InitialSpeed;
	bIdle = false;
	bAttack = false;
	//RetreatLocation = CalculateEngagedLocation();
	GetWorldTimerManager().SetTimer(ResumeTimerHandle, this, &AEnemyDroneBaseActor::Resume, 0.1f, false, ResumeDelay);
}

void AEnemyDroneBaseActor::Resume()
{
	//StartTimers();
	bIdle = true;
}

void AEnemyDroneBaseActor::ResumeInitialSpeed()
{
	CurrentSpeed = InitialSpeed;
}


/** Checks if drone location is in range to initiate attack*/
bool AEnemyDroneBaseActor::IsWithinAttackArea() const
{
	return (GetActorLocation() - BoundCenterPosition).Length() < OuterBoundRadius && (GetActorLocation() - BoundCenterPosition).Length() > InnerBoundRadius;
}

/* 
void AEnemyDroneBaseActor::HasLostPlayer() 
{
	if (FVector::Dist(GetActorLocation(), PlayerLocation) > MaxDistanceToPlayer)
	{
		CurrentTargetLocation = HomeLocation;
	}
}
*/

void AEnemyDroneBaseActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		if (Cast<AAcopalypsCharacter>(OtherActor))
		{
			UGameplayStatics::ApplyDamage(OtherActor, 50.f, GetWorld()->GetFirstPlayerController(), this,nullptr);
		}

		if (Cast<AProjectile>(OtherActor) && Cast<AProjectile>(OtherActor)->Owner == Cast<AAcopalypsCharacter>(OtherActor))
		{
			CurrentSpeed = 0.3f;
			GetWorldTimerManager().SetTimer(ResumeSpeedHandle, this, &AEnemyDroneBaseActor::Resume,  0.1f, false, ResumeSpeedDelay); // 0.3f;
		}
	}
}

bool AEnemyDroneBaseActor::IsDead() const
{
	return HealthComponent->IsDead();
}

float AEnemyDroneBaseActor::GetHealthPercent() const
{
	return HealthComponent->GetHealthPercent();
}

float AEnemyDroneBaseActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if(IsDead()) return DamageApplied;
	DamageApplied = FMath::Min(HealthComponent->GetHealth(), DamageApplied);
	HealthComponent->SetHealth(HealthComponent->GetHealth() - DamageApplied);
	
	if(IsDead())
	{
		OnDeathEvent();
		//CombatManager->RemoveDrone(this);
		Destroy();
	}
	return DamageApplied;
}


