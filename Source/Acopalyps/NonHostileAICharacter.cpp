// Fill out your copyright notice in the Description page of Project Settings.


#include "NonHostileAICharacter.h"
#include "AcopalypsPrototypeGameModeBase.h"
#include "NonHostileAIController.h"
#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ANonHostileAICharacter::ANonHostileAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set mesh to enemy mesh, and sets collision presets
	CharacterMesh = GetMesh();

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

// Called when the game starts or when spawned
void ANonHostileAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if(!GetController())
	{
		SpawnDefaultController();
	}
	ANonHostileAIController* AIController = Cast<ANonHostileAIController>(GetController());
	AIController->Initialize();
}

// Called every frame
void ANonHostileAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANonHostileAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ANonHostileAICharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if( IsDead() )
	{
		return DamageApplied;
	}
	DamageApplied = FMath::Min(HealthComponent->GetHealth(), DamageApplied);
	HealthComponent->SetHealth(HealthComponent->GetHealth() - DamageApplied);
	UE_LOG(LogTemp, Display, TEXT("health: %f"), HealthComponent->GetHealth());
	
	if( IsDead())
	{
		if (AAcopalypsPrototypeGameModeBase* PrototypeGameModeBase = GetWorld()->GetAuthGameMode<AAcopalypsPrototypeGameModeBase>())
		{
			PrototypeGameModeBase->PawnKilled(this);
		}
		OnDeath();
		RagDoll();
		//if(Manager)
		//	Manager->RemoveEnemy(this);
		DetachFromControllerPendingDestroy();
		GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT(" Died: %s "), *GetName()));
	}
	return DamageApplied;
}

bool ANonHostileAICharacter::IsDead() const
{
	return HealthComponent->IsDead();
}

float ANonHostileAICharacter::GetHealthPercent() const
{
	return HealthComponent->GetHealthPercent();
}

void ANonHostileAICharacter::RagDoll()
{
	RagDoll(FVector::ZeroVector);
}


void ANonHostileAICharacter::RagDoll(FVector ForceDirection)
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("RagDoll");
	GetMesh()->AddForceToAllBodiesBelow(ForceDirection, TEXT("pelvis"), true);
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	Cast<ANonHostileAIController>(GetController())->SetIsRagdoll(true);
	GetWorldTimerManager().SetTimer(RagDollTimerHandle, this, &ANonHostileAICharacter::UnRagDoll, 3.f, false, 1.f);
}


void ANonHostileAICharacter::UnRagDoll()
{
	if(IsDead()) return;
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	GetCapsuleComponent()->SetCollisionProfileName("Enemy");
	Cast<ANonHostileAIController>(GetController())->SetIsRagdoll(false);
	GetCapsuleComponent()->SetWorldLocation(GetMesh()->GetComponentLocation(), false, nullptr, ETeleportType::ResetPhysics);
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0), false, nullptr, ETeleportType::ResetPhysics);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90), false, nullptr, ETeleportType::ResetPhysics);
}
