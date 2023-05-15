// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDroneBaseActor.h"


#include "AcopalypsCharacter.h"
#include "AcopalypsProjectile.h"
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

	// Binding function to start of overlap with player
	SphereColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyDroneBaseActor::OnOverlapBegin);
	
	CurrentSpeed = InitialSpeed;

	GenerateNewRelativePosition();
	
	bIdle = true;
	bAttack = false;
}

void AEnemyDroneBaseActor::StartTimers()
{
	GetWorldTimerManager().SetTimer(UpdateCurrentObjectiveTimerHandle, this, &AEnemyDroneBaseActor::UpdateCurrentObjective, UpdateCurrentObjectiveDelay, true, 0.1f); // 0.2f;
	GetWorldTimerManager().SetTimer(CheckAttackBoundsTimerHandle, this, &AEnemyDroneBaseActor::CheckAttackPotential, CheckAttackPotentialDelay, true, 0.1f); //0.2f;
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

	AdjustMovementForCollision();

	// Rotation
	const FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerLocation);
	SetActorRotation(NewRotation + FRotator(0, -90, 0));
	
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
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CurrentTargetLocation = EngagedLocation"));

		CurrentTargetLocation = EngagedLocation;
		
	}
	else if (!bIdle && bAttack)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CurrentTargetLocation = PlayerLocation"));

		CurrentTargetLocation = PlayerLocation;
	}
	else if (!bIdle && !bAttack)
	{
		CalculateEngagedLocation();
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CurrentTargetLocation = EngagedLocation"));

		CurrentTargetLocation = EngagedLocation;
	}
}

/** Updates location from which to start attack*/
void AEnemyDroneBaseActor::CalculateEngagedLocation() 
{
	BoundCenterPosition = PlayerLocation;
	if (DebugAssist)
	{
		DrawDebugSphere(GetWorld(), PlayerLocation, InnerBoundRadius, 30, FColor::Blue, false,0.2f);
		DrawDebugSphere(GetWorld(), PlayerLocation, OuterBoundRadius, 30, FColor::Green, false,0.2f);

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
	
	EngagedLocation = NewLocation;
	DrawDebugSphere(GetWorld(), EngagedLocation, 20.f, 30, FColor::Black, false,0.2f);
}

void AEnemyDroneBaseActor::GenerateNewRelativePosition()
{ 
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

	bool bHit;
	if (DebugAssist)
	{
		bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), GetActorLocation(),
		NewLocation, DroneRadius , ObjectTypes, true, IgnoreActors, EDrawDebugTrace::ForDuration,
		HitResult, true, FColor::Green, FLinearColor::Red,  0.2f);
	}
	else
	{
		bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), GetActorLocation(),
		NewLocation, DroneRadius , ObjectTypes, true, IgnoreActors, EDrawDebugTrace::None,
		HitResult, true, FColor::Transparent, FLinearColor::Transparent,  0.2f);
	}
	if (bHit)
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

	// Distance vector between drone and hitpoint aka hypotenusan
	const FVector DroneToGoal = (HitPoint - GetActorLocation());

	// Projection of DroneToGoal on SurfaceNormal
	const double ProjectionOnSurfNorm = -SurfaceNormal.Dot(DroneToGoal);

	if (ProjectionOnSurfNorm > 0.95)
	{
		GenerateNewRelativePosition();
	}

	// reflection vector from hit point
	FVector ReflectionVector = (2 * ProjectionOnSurfNorm * SurfaceNormal * DroneToGoal.Size() - DroneToGoal);
	ReflectionVector = ReflectionVector.GetSafeNormal() * FMath::Clamp(ReflectionVector.Size(), 0, CollisionAvoidanceOffset);
	AdjustedLocation += ReflectionVector;

	if (DebugAssist)
	{
		DrawDebugSphere(GetWorld(), HitPoint, 30.f, 30, FColor::Black, true,0.2f);
		DrawDebugSphere(GetWorld(), AdjustedLocation, 30.f, 30, FColor::Orange, true,0.2f);
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
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("PrepareForAttack"));
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyDroneBaseActor::Attack, 0.1f, false, AttackDelay);
}

/** Runs when PrepareForAttack is done, starts drones unique attack pattern*/
void AEnemyDroneBaseActor::Attack()
{
	OnAttackEvent();
	CurrentSpeed = AttackSpeed;
	bAttack = true;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Attack"));
	GetWorldTimerManager().SetTimer(RetreatTimerHandle, this, &AEnemyDroneBaseActor::Retreat, 0.1f, false, RetreatDelay);
}

/** Runs after attack, to get a new target position */
void AEnemyDroneBaseActor::Retreat()
{
	OnRetreatEvent();
	CurrentSpeed = InitialSpeed;
	bIdle = false;
	bAttack = false;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Retreat"));
	GetWorldTimerManager().SetTimer(ResumeTimerHandle, this, &AEnemyDroneBaseActor::Resume, 0.1f, false, ResumeDelay);
}

void AEnemyDroneBaseActor::Resume()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Resume"));
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

void AEnemyDroneBaseActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		if (Cast<AAcopalypsCharacter>(OtherActor))
		{
			UGameplayStatics::ApplyDamage(OtherActor, 50.f, GetWorld()->GetFirstPlayerController(), this,nullptr);
		}
		/*
		AProjectile* Projectile = Cast<AProjectile>(OtherActor);
		if (Projectile)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Hit with projectile")));

		}
		*/
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


/*
 * Get the hit point and surface normal
	const FVector HitPoint = SweepHitResult.ImpactPoint;

	// Calculate a location around collision
	const FVector SurfaceNormal = SweepHitResult.ImpactNormal;
	FVector AdjustedLocation = HitPoint +  SurfaceNormal * CollisionAvoidanceOffset;

	// Distance vector between drone and player location aka hypotenusan
	//const FVector DroneToGoal = ((PlayerLocation + RelativePositionToPLayer) - GetActorLocation()).GetSafeNormal();

	//const FVector DroneToGoal = ((GoalLocation) - GetActorLocation()).GetSafeNormal();
	const FVector DroneToGoal = (HitPoint - GetActorLocation());


	// Projection of DroneToPlayer on SurfaceNormal
	const double ProjectionOnSurfNorm = -SurfaceNormal.Dot(DroneToGoal);

	if (ProjectionOnSurfNorm > 0.95)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("ProjectionOnSurfNorm %f" ), ProjectionOnSurfNorm));
		GenerateNewRelativePosition();
	}

	/*
	// Magnitude of distance vector aka långa katetern
	//const double DistanceMagnitude = DroneToGoal.Size();
	
	//const FVector AdjustedDirection = DroneToGoal + SurfaceNormal * DistanceMagnitude * ProjectionOnSurfNorm;
	
	FVector ReflectionVector = (2 * ProjectionOnSurfNorm * SurfaceNormal * DroneToGoal.Size() - DroneToGoal);

	const double DistanceMagnitude = DroneToGoal.Size();
	ReflectionVector = ReflectionVector.GetSafeNormal() * FMath::Clamp(ReflectionVector.Size(), 0, CollisionAvoidanceOffset);
	//const FVector AdjustedDirection = ReflectionVector.GetSafeNormal();
	//const FVector AdjustedDirection = DroneToGoal + ReflectionVector.GetSafeNormal() * DistanceMagnitude * ProjectionOnSurfNorm;
	//AdjustedLocation += AdjustedDirection;
	AdjustedLocation += ReflectionVector;
	//DrawDebugSphere(GetWorld(), AdjustedLocation, 30.f, 30, FColor::Orange, true,0.2f);

	if (DebugAssist)
	{
		DrawDebugSphere(GetWorld(), HitPoint, 30.f, 30, FColor::Black, true,0.2f);
		DrawDebugSphere(GetWorld(), AdjustedLocation, 30.f, 30, FColor::Orange, true,0.2f);
	}
	return AdjustedLocation;
 
 */