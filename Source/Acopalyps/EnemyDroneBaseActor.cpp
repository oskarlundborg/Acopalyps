// Fill out your copyright notice in the Description page of Project Settings.

/** @author Isabel Mirella Diaz Johansson */

#include "EnemyDroneBaseActor.h"


#include "AcopalypsCharacter.h"
#include "HealthComponent.h"
#include "Components/SphereComponent.h"
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
	if( PlayerCharacter != nullptr )
	{
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerCharacter->GetActorLocation()));

		// Calculate the location of the AI character relative to the player, and rotation
		const FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
		const FRotator InitialRotation = DirectionToPlayer.ToOrientationRotator();
		SetActorRotation(InitialRotation);
	}
	// Start timers to update target location and to check for current state
	StartTimers();

	// Binding function to start of overlap with player
	SphereColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyDroneBaseActor::OnOverlapBegin);
	
	TargetSpeed = InitialSpeed;

	GenerateNewRelativePosition();
	
	bIdle = true;
	bAttack = false;
}

/** Starts looping timers*/
void AEnemyDroneBaseActor::StartTimers()
{
	GetWorldTimerManager().SetTimer(UpdateCurrentObjectiveTimerHandle, this, &AEnemyDroneBaseActor::UpdateCurrentObjective, UpdateCurrentObjectiveDelay, true, 0.1f); // 0.2f;
	GetWorldTimerManager().SetTimer(CheckAttackBoundsTimerHandle, this, &AEnemyDroneBaseActor::CheckAttackPotential, CheckAttackPotentialDelay, true, 0.1f); //0.2f;
}

// Called every frame
void AEnemyDroneBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if( PlayerCharacter != nullptr )
	{
	    PlayerLocation = PlayerCharacter->GetActorLocation();
	    PlayerLocation.Z += MinHeightAbovePlayer;
	    PlayerRotation = PlayerCharacter->GetActorRotation();
	}
	if (!bIsDead)
	{
		MoveTowardsLocation(DeltaTime);
	}
}

/** Moves actor towards a location in world*/
void AEnemyDroneBaseActor::MoveTowardsLocation(float DeltaTime)
{
	OnDroneMovement();

	AdjustMovementForCollision();

	CurrentSpeed = FMath::Lerp(CurrentSpeed, TargetSpeed, 0.1);

	// Rotation
	const FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerLocation);
	SetActorRotation(FRotator(0, NewRotation.Yaw -90, 0));
	
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

/** Decides which location to move towards based on states*/
void AEnemyDroneBaseActor::UpdateCurrentObjective()
{
	if (!bAttack)
	{
		CalculateEngagedLocation();
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
		if(Counter > 1)
			GenerateNewRelativePosition();
		NewLocation = BoundCenterPosition + RelativePositionToPLayer;
		NewLocation = FVector(NewLocation.X, NewLocation.Y, FMath::Clamp(NewLocation.Z, PlayerLocation.Z, PlayerLocation.Z + MaxHeightAbovePlayer)); //kanske lägga till min height player
		Counter++;
	}
	while (IsTargetLocationValid(NewLocation) && IsWithinPlayerInnerBounds(NewLocation) && Counter <=10);
	
	EngagedLocation = NewLocation;
	if (DebugAssist) DrawDebugSphere(GetWorld(), EngagedLocation, 20.f, 30, FColor::Purple, false,0.2f);
}

/** Updates location to move towards during attack*/
void AEnemyDroneBaseActor::CalculateAttackLocation()
{
	FVector DirectionToPlayer = PlayerLocation - GetActorLocation();
	DirectionToPlayer = DirectionToPlayer.GetSafeNormal();
	const float DistanceToPlayer = FVector::Distance(PlayerLocation, GetActorLocation());
	AttackLocation = GetActorLocation() + DirectionToPlayer * (DistanceToPlayer + DistanceBehindPlayer);
	if (IsTargetLocationValid(AttackLocation))
	{
		AttackLocation = PlayerLocation;
	}
}

/** Updates location to move towards during retreat*/
void AEnemyDroneBaseActor::CalculatePrepareAttackLocation()
{
	if (GetActorLocation().Z != PlayerLocation.Z)
	{
		PrepareAttackLocation = FVector(CurrentTargetLocation.X, CurrentTargetLocation.Y, PlayerLocation.Z);
	}
}

/** Generate new relative position to player*/
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

/** Adjusts movement depending on collision*/
void AEnemyDroneBaseActor::AdjustMovementForCollision()
{
	if (CollisionOnPathToTarget(GetActorLocation(),CurrentTargetLocation))
	{
		CurrentTargetLocation = GetAdjustedLocation();
	}
}

/** Performs a sphere trace, returns true if colliding object found between target location and chosen location to sweep from to avoid collision*/
bool AEnemyDroneBaseActor::CollisionOnPathToTarget(FVector SweepStartLocation, FVector NewLocation)
{
	FHitResult HitResult;
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);
	
	// Define the object types to trace against
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	//ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	//ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel3)); // Enemy
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel9)); // Drone

	TArray<AActor*> IgnoreActors = TArray<AActor*>();
	IgnoreActors.Add(this);

	bool bHit;
	if (DebugAssist)
	{
		bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), SweepStartLocation,
		NewLocation, DroneRadius , ObjectTypes, true, IgnoreActors, EDrawDebugTrace::ForDuration,
		HitResult, true, FColor::Green, FLinearColor::Red,  0.2f);
	}
	else
	{
		bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), SweepStartLocation,
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

/** Calculates and returns closest location that avoids collision */
FVector AEnemyDroneBaseActor::GetAdjustedLocation() 
{
	float AvoidingOffset;
	if (bIsPreparingForAttack)
	{
		AvoidingOffset = 50.f;
	}
	else
	{
		AvoidingOffset = CollisionAvoidanceOffset;
	}
	
	// Get the hit point and surface normal
	const FVector HitPoint = SweepHitResult.ImpactPoint;
	const FVector SurfaceNormal = SweepHitResult.ImpactNormal;
	
	// Calculate a location around collision
	FVector AdjustedLocation = HitPoint +  SurfaceNormal * AvoidingOffset;

	// Distance vector between drone and hitpoint aka hypotenusan
	const FVector DroneToGoal = (HitPoint - GetActorLocation());

	// Projection of DroneToGoal on SurfaceNormal
	const double ProjectionOnSurfNorm = -SurfaceNormal.Dot(DroneToGoal);

	if (ProjectionOnSurfNorm > 0.99)
	{
		GenerateNewRelativePosition();
	}

	// reflection vector from hit point
	FVector ReflectionVector = (2 * ProjectionOnSurfNorm * SurfaceNormal * DroneToGoal.Size() - DroneToGoal);
	ReflectionVector = ReflectionVector.GetSafeNormal() * FMath::Clamp(ReflectionVector.Size(), 0, AvoidingOffset); 
	AdjustedLocation += ReflectionVector;
	
	if (bIdle && CollisionOnPathToTarget(GetActorLocation(),EngagedLocation))
	{
		AdjustedLocation.Z += 50.f; //borde vara en global variabel
	}

	if (DebugAssist)
	{
		DrawDebugSphere(GetWorld(), HitPoint, 30.f, 30, FColor::Black, false,0.2f);
		DrawDebugSphere(GetWorld(), AdjustedLocation, 30.f, 30, FColor::Orange, false,0.2f);
	}
	return AdjustedLocation;
}

/** Performs a line trace, returns if location is viable, and that movement to the point is possible */
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
	bIsPreparingForAttack = true;
	CalculatePrepareAttackLocation();
	CurrentTargetLocation = PrepareAttackLocation;
	CurrentSpeed = 200.f;
	TargetSpeed = 200.f;
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyDroneBaseActor::Attack, 0.1f, false, AttackDelay);
}

/** Runs when PrepareForAttack is done, starts drones unique attack pattern*/
void AEnemyDroneBaseActor::Attack()
{
	OnAttackEvent();
	bAttack = true;
	bIsPreparingForAttack = false;
	CalculateAttackLocation();
	CurrentTargetLocation = AttackLocation;
	CurrentSpeed = AttackSpeed;
	TargetSpeed = AttackSpeed;
	GetWorldTimerManager().SetTimer(RetreatTimerHandle, this, &AEnemyDroneBaseActor::Retreat, 0.1f, false, RetreatDelay);
}

/** Runs after attack, to get a new target position */
void AEnemyDroneBaseActor::Retreat()
{
	OnRetreatEvent();
	TargetSpeed = InitialSpeed;
	bAttack = false;
	GetWorldTimerManager().SetTimer(ResumeTimerHandle, this, &AEnemyDroneBaseActor::Resume, 0.1f, false, ResumeDelay);
}

/** Resumes drone to idle behavior*/
void AEnemyDroneBaseActor::Resume()
{
	bIdle = true;
}

/** Sets drone speed to initial speed*/
void AEnemyDroneBaseActor::ResumeInitialSpeed()
{
	TargetSpeed = InitialSpeed;
}

/** Runs when drone dies*/
void AEnemyDroneBaseActor::DoDeath()
{
	OnDeathEvent();
	TargetSpeed = 0.00f;
	CurrentSpeed = 0.00f;
	GetWorldTimerManager().ClearTimer(UpdateCurrentObjectiveTimerHandle);
	GetWorldTimerManager().ClearTimer(CheckAttackBoundsTimerHandle);
	bIsDead = true;
	SphereColliderComponent->SetCollisionProfileName("Ragdoll");
	SphereColliderComponent->SetSimulatePhysics(true);
	SphereColliderComponent->SetEnableGravity(true);
	SphereColliderComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemyDroneBaseActor::OnOverlapBegin);
	SphereColliderComponent->OnComponentHit.AddDynamic(this, &AEnemyDroneBaseActor::OnDeathGroundHit);
	DroneMesh->SetCollisionProfileName("Ragdoll");
	DroneMesh->SetSimulatePhysics(true);
	DroneMesh->SetEnableGravity(true);
	if(CombatManager) CombatManager->RemoveDrone(this);
}

/** Destroys drone after a some time*/
void AEnemyDroneBaseActor::DestroyDrone()
{
	Destroy();
}

/** Checks if drone location is in range to initiate attack*/
bool AEnemyDroneBaseActor::IsWithinAttackArea() const
{
	return (GetActorLocation() - BoundCenterPosition).Length() < OuterBoundRadius && (GetActorLocation() - BoundCenterPosition).Length() > InnerBoundRadius;
}

/** Checks if drone location is too close to player aka within inner bounds */
bool AEnemyDroneBaseActor::IsWithinPlayerInnerBounds(const FVector& LocationToCheck) const
{
	return (LocationToCheck - BoundCenterPosition).Length() < InnerBoundRadius;
}

/** Called when drone starts overlapping something */
void AEnemyDroneBaseActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		if (Cast<AAcopalypsCharacter>(OtherActor))
		{
			OnDroneHitPlayerEvent();
			UGameplayStatics::ApplyDamage(OtherActor, 50.f, GetWorld()->GetFirstPlayerController(), this,nullptr);
		}
	}
}

/** Called when drone dies and hits ground */
void AEnemyDroneBaseActor::OnDeathGroundHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr)
	{
		if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_WorldStatic)
		{
			OnDeathCrashEvent();
			GetWorldTimerManager().SetTimer(DestructionTimerHandle, this, &AEnemyDroneBaseActor::DestroyDrone, 0.1f, false, DestructionDelay);
			SphereColliderComponent->OnComponentHit.RemoveDynamic(this, &AEnemyDroneBaseActor::OnDeathGroundHit);
		}
	}
}

/** Returns if actor is dead */
bool AEnemyDroneBaseActor::IsDead() const
{
	return HealthComponent->IsDead();
}

/** Returns actors remaining health */
float AEnemyDroneBaseActor::GetHealthPercent() const
{
	return HealthComponent->GetHealthPercent();
}

/** Called upon when object channel weapon collider collides with drone */
float AEnemyDroneBaseActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if(IsDead()) return DamageApplied;
	DamageApplied = FMath::Min(HealthComponent->GetHealth(), DamageApplied);
	HealthComponent->SetHealth(HealthComponent->GetHealth() - DamageApplied);

	TargetSpeed = 0.02f;
	GetWorldTimerManager().SetTimer(ResumeSpeedHandle, this, &AEnemyDroneBaseActor::ResumeInitialSpeed, 0.1f, false, ResumeSpeedDelay);

	
	if(IsDead())
	{
		DoDeath();
	}
	return DamageApplied;
}

