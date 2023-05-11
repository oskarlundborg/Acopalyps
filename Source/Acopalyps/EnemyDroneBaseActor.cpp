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
	FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	FRotator InitialRotation = DirectionToPlayer.ToOrientationRotator();
	SetActorRotation(InitialRotation);

	// Start timers to update target location and to check for current state
	StartTimers();
	
	// Binding function to start of overlap with player
	SphereColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyDroneBaseActor::OnOverlapBegin);

	
	SphereColliderComponent->OnComponentHit.AddDynamic(this, &AEnemyDroneBaseActor::OnHit);
	this->OnActorHit.AddDynamic(this, &AEnemyDroneBaseActor::OnDroneHit);
	
	CurrentSpeed = InitialSpeed;

	GenerateNewRelativePosition();
	
	bIdleState = true;

}

void AEnemyDroneBaseActor::StartTimers()
{
	GetWorldTimerManager().SetTimer(UpdateEngagedLocationTimerHandle, this, &AEnemyDroneBaseActor::UpdateTargetLocation, UpdateEngagedTargetDelay, true, 0.1f);
	GetWorldTimerManager().SetTimer(CheckAttackBoundsTimerHandle, this, &AEnemyDroneBaseActor::CalculateCurrentState, CheckDroneStateDelay, true, 0.1f);
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

	Direction = TargetLocation - GetActorLocation();
	Direction.Normalize();

	// Calculate the distance between the current location and the target location
	const float Distance = FVector::Distance(TargetLocation, GetActorLocation());

	// Calculate distance the actor can move in this step based on the speed variable
	float MoveDistance = CurrentSpeed * DeltaTime;
	MoveDistance = FMath::Min(MoveDistance, Distance);
	const FVector NewLocation = GetActorLocation() + Direction * MoveDistance;
	SetActorLocation(NewLocation);
}

/** Updates location from which to start attack*/
FVector AEnemyDroneBaseActor::GetNewEngagedLocation() 
{
	FVector NewLocation;
	int Counter = 0;
	do
	{
		NewLocation = BoundCenterPosition + RelativePositionToPLayer;
		NewLocation = FVector(NewLocation.X, NewLocation.Y, FMath::Clamp(NewLocation.Z, PlayerLocation.Z + MinHeightAbovePlayer, PlayerLocation.Z + MaxHeightAbovePlayer));
		Counter++;
	}
	while (IsTargetLocationValid(NewLocation) && Counter <=10);
	
	if (CollisionOnPathToTarget(NewLocation))
	{
		NewLocation = GetAdjustedLocation();
	}
	return NewLocation;
}

void AEnemyDroneBaseActor::GenerateNewRelativePosition()
{
	RelativePositionToPLayer = FVector(UKismetMathLibrary::RandomUnitVector()) * UKismetMathLibrary::RandomFloatInRange(InnerBoundRadius, OuterBoundRadius);
}


/** Updates bounds for location to move toward*/
void AEnemyDroneBaseActor::UpdateTargetLocation()
{
	//BoundCenterPosition = PlayerLocation + PlayerCharacter->GetActorForwardVector() * OuterBoundRadius + FVector(0, 0, MinHeightAbovePlayer);
	BoundCenterPosition = PlayerLocation;
	const FColor SphereColor = FColor::Green;

	TargetLocation = GetNewEngagedLocation();
}

/** Calculates if drone should attack or following player*/
void AEnemyDroneBaseActor::CalculateCurrentState()
{
	if (IsWithinAttackArea() && IsTargetLocationValid(TargetLocation) && bIdleState && !bAttackState)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Prepare attack")));
		PrepareForAttack();
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
	//ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel9)); //Drone

	TArray<AActor*> IgnoreActors = TArray<AActor*>();
	IgnoreActors.Add(this);

	if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), GetActorLocation(),
		NewLocation, DroneRadius , ObjectTypes, true, IgnoreActors, EDrawDebugTrace::None,
		HitResult, true, FColor::Transparent, FLinearColor::Transparent,  1.f)
)
	{
		SweepHitResult = HitResult;
		return true;
	}
	return false;
}

FVector AEnemyDroneBaseActor::GetAdjustedLocation() 
{
	// Get the hit point and surface normal
	const FVector HitPoint = SweepHitResult.ImpactPoint;

	// Calculate a location around collision
	const FVector SurfaceNormal = SweepHitResult.ImpactNormal;
	FVector AdjustedLocation = HitPoint +  SurfaceNormal * CollisionAvoidanceOffset;

	// Distance vector between drone and player location aka hypotenusan
	const FVector DroneToPlayer = ((PlayerLocation + RelativePositionToPLayer) - GetActorLocation()).GetSafeNormal();

	// Projection of DroneToPlayer on SurfaceNormal
	const double ProjectionOnSurfNorm = -SurfaceNormal.Dot(DroneToPlayer);

	if (ProjectionOnSurfNorm > 0.95)
	{
		GenerateNewRelativePosition();
	}
	
	// Magnitude of distance vector aka långa katetern
	const double DistanceMagnitude = DroneToPlayer.Size();
	
	const FVector AdjustedDirection = DroneToPlayer + SurfaceNormal * DistanceMagnitude * ProjectionOnSurfNorm;
	AdjustedLocation += AdjustedDirection;
	
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

void AEnemyDroneBaseActor::PrepareForAttack()
{
	OnPrepareForAttackEvent();
	bIdleState = false;
	CurrentSpeed = 0.1f;
	//logic for when drone is standing still
	StopTimers();
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyDroneBaseActor::Attack, 0.1f, false, AttackDelay);
}

void AEnemyDroneBaseActor::Attack()
{
	CurrentSpeed = AttackSpeed;
	TargetLocation = PlayerLocation;
	bAttackState = true;
	GetWorldTimerManager().SetTimer(RetreatTimerHandle, this, &AEnemyDroneBaseActor::Retreat, 0.1f, false, RetreatDelay);
}

void AEnemyDroneBaseActor::Retreat()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("retreat!!!")));
	CurrentSpeed = InitialSpeed;
	bIdleState = false;
	bAttackState = false;
	TargetLocation = GetNewEngagedLocation();
	GetWorldTimerManager().SetTimer(ResumeTimerHandle, this, &AEnemyDroneBaseActor::Resume, 0.1f, false, ResumeDelay);
}

void AEnemyDroneBaseActor::Resume()
{
	StartTimers();
	bIdleState = true;
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
		UE_LOG(LogTemp, Display, TEXT("Overlapping start with: %s"), *OtherActor->GetClass()->GetName());

		if (Cast<AAcopalypsCharacter>(OtherActor))
		{
			UGameplayStatics::ApplyDamage(OtherActor, 50.f, GetWorld()->GetFirstPlayerController(), this,nullptr);
			//UE_LOG(LogTemp, Display, TEXT("Overlapping start with: %s"), *OtherActor->GetClass()->GetName());
		}
	}
}


void AEnemyDroneBaseActor::OnHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
	)
{
	//HitTriggerEvent(Hit);
	AActor* HitActor = Hit.GetActor();
	AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(OtherActor);
	if (Player)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player hit")));	

	}
	if(HitActor != nullptr && HitActor != this)
	{
		UGameplayStatics::ApplyDamage(HitActor, 50.f, GetWorld()->GetFirstPlayerController(), this,nullptr);
		//const AActor* ConstHitActor = HitActor;
		UE_LOG(LogTemp, Display, TEXT("Hit with drone: %s"), *OtherActor->GetClass()->GetName());
	}
}

void AEnemyDroneBaseActor::OnDroneHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse,
	const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(OtherActor);
	if (Player)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player hit")));	

	}
	if(HitActor != nullptr && HitActor != this)
	{
		UGameplayStatics::ApplyDamage(HitActor, 50.f, GetWorld()->GetFirstPlayerController(), this,nullptr);
		//const AActor* ConstHitActor = HitActor;
		UE_LOG(LogTemp, Display, TEXT("Hit with drone: %s"), *OtherActor->GetClass()->GetName());
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
	UE_LOG(LogTemp, Display, TEXT("health: %f"), HealthComponent->GetHealth());
	
	if(IsDead())
	{
		OnDeathEvent();
		Destroy();
		GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT(" Died: %s "), *GetName()));
	}
	//TakeDamageTriggerEvent(DamageAmount, DamageCauser->GetActorLocation(), DamageEvent, EventInstigator, DamageCauser);
	return DamageApplied;
}


