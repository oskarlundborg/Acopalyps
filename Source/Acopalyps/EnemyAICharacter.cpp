// Fill out your copyright notice in the Description page of Project Settings.

/** @author Jonathan Rubensson */


#include "EnemyAICharacter.h"

#include "AcopalypsCharacter.h"
#include "AcopalypsPrototypeGameModeBase.h"
#include "EnemyAIController.h"
#include "HealthComponent.h"
#include "CombatManager.h"
#include "CoverPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
AEnemyAICharacter::AEnemyAICharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set mesh to enemy mesh, and sets collision presets
	CharacterMesh = GetMesh();
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

// Called when the game starts or when spawned
void AEnemyAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if(!GetController())
	{
		SpawnDefaultController();
	}
	
	if( GunClass != nullptr )
	{
		Gun = GetWorld()->SpawnActor<AGun>(GunClass);
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		Gun->AttachToComponent(GetMesh(), AttachmentRules, TEXT("GunSocket"));
		Gun->SetOwner(this);
		Gun->SetActorRelativeRotation(FRotator(0, -90, 0));
	}
}

// Called every frame
void AEnemyAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEnemyAICharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	InitializeController();
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if(IsDead()) return DamageApplied;
	DamageApplied = FMath::Min(HealthComponent->GetHealth(), DamageApplied);
	HealthComponent->SetHealth(HealthComponent->GetHealth() - DamageApplied);
	UE_LOG(LogTemp, Display, TEXT("health: %f"), HealthComponent->GetHealth());
	if(Manager)
	{
		Manager->StartCombatMode();
	}
	if(IsDead())
	{
		if (AAcopalypsPrototypeGameModeBase* PrototypeGameModeBase = GetWorld()->GetAuthGameMode<AAcopalypsPrototypeGameModeBase>())
		{
			PrototypeGameModeBase->PawnKilled(this);
		}
		OnDeath();
		RagDoll();
		AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
		if(Controller)
		{
			ACoverPoint* CoverPoint = Cast<ACoverPoint>(AIController->GetBlackboardComponent()->GetValueAsObject("Cover"));
			if(CoverPoint) CoverPoint->bIsOccupied = false;
		}
		if(Manager)
		{
			Manager->RemoveEnemy(this);
		}
		DetachFromControllerPendingDestroy();
	}
	return DamageApplied;
}

/** Checks health components state, if character is dead */
bool AEnemyAICharacter::IsDead() const
{
	return HealthComponent->IsDead();
}

/** Runs when character shoots towards player character, decided in Behavior tree*/
void AEnemyAICharacter::Shoot()
{
	if( Gun->CurrentMag <= 0 )
	{
		Gun->Reload();
	}
	if(CurrentMag <= 0) Gun->Reload();
	FireEnemyTriggerEvent();
	Cast<AEnemyAIController>(GetController())->SetAim();
	Gun->Fire(Rapid);
	FireEnemyTriggerEvent();
}

/** Gets health components current health in percent */
float AEnemyAICharacter::GetHealthPercent() const
{
	return HealthComponent->GetHealthPercent();
}

/** Sets physics and collision of actor to rag-doll-settings, without adding force */
void AEnemyAICharacter::RagDoll()
{
	RagDoll(FVector::ZeroVector);
}

/** Sets physics and collision of actor to rag-doll-settings, and applies force on physics body */
void AEnemyAICharacter::RagDoll(FVector ForceDirection)
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("RagDoll");
	GetMesh()->AddForceToAllBodiesBelow(ForceDirection, TEXT("pelvis"), true);
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	Cast<AEnemyAIController>(GetController())->SetIsRagdoll(true);
	GetWorldTimerManager().SetTimer(RagDollTimerHandle, this, &AEnemyAICharacter::UnRagDoll, 3.f, false, 1.f);
}

/** Initializes EnemyAIController */
void AEnemyAICharacter::InitializeController()
{
	if(bIsInitialized) return;
	bIsInitialized = true;
	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if(AIController)
	{
		AIController->Initialize();
	}
}

/** Resets physics and collision presets to idle after ragdoll-method call*/
void AEnemyAICharacter::UnRagDoll()
{
	if(IsDead()) return;
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	GetCapsuleComponent()->SetCollisionProfileName("Enemy");
	Cast<AEnemyAIController>(GetController())->SetIsRagdoll(false);
	GetCapsuleComponent()->SetWorldLocation(GetMesh()->GetComponentLocation(), false, nullptr, ETeleportType::ResetPhysics);
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0), false, nullptr, ETeleportType::ResetPhysics);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90), false, nullptr, ETeleportType::ResetPhysics);
}
