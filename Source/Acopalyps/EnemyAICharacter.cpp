// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAICharacter.h"
#include "AcopalypsPrototypeGameModeBase.h"
#include "EnemyAIController.h"
#include "HealthComponent.h"
#include "Algo/Rotate.h"
#include "Components/CapsuleComponent.h"

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
	

	// Sets mesh for gun to socket on Character Mesh, and sets collision presets
	/*
	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("GunSocket"));
	Gun->SetOwner(this);
	*/
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
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if(IsDead()) return DamageApplied;
	DamageApplied = FMath::Min(HealthComponent->GetHealth(), DamageApplied);
	HealthComponent->SetHealth(HealthComponent->GetHealth() - DamageApplied);
	UE_LOG(LogTemp, Display, TEXT("health: %f"), HealthComponent->GetHealth());

	
	if(IsDead())
	{
		if (AAcopalypsPrototypeGameModeBase* PrototypeGameModeBase = GetWorld()->GetAuthGameMode<AAcopalypsPrototypeGameModeBase>())
		{
			PrototypeGameModeBase->PawnKilled(this);
		}
		RagDoll();
		DetachFromControllerPendingDestroy();
		GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT(" Died: %s "), *GetName()));
	}
	return DamageApplied;
}

bool AEnemyAICharacter::IsDead() const
{
	return HealthComponent->IsDead();
}

void AEnemyAICharacter::Shoot()
{
	GEngine->AddOnScreenDebugMessage(-1,2.f, FColor::Red, FString::Printf(TEXT(" Enemy Shooting")));
}
float AEnemyAICharacter::GetHealthPercent() const
{
	return HealthComponent->GetHealthPercent();
}

void AEnemyAICharacter::RagDoll()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("RagDoll");
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	Cast<AEnemyAIController>(GetController())->SetIsRagdoll(true);
	GetWorldTimerManager().SetTimer(RagDollTimerHandle, this, &AEnemyAICharacter::UnRagDoll, 3.f, false, 1.f);
}

void AEnemyAICharacter::RagDoll(FVector ForceDirection)
{
	RagDoll();
	GetMesh()->AddForceToAllBodiesBelow(ForceDirection, TEXT("pelvis"), true);
}


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