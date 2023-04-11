// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAICharacter.h"
#include "Gun.h"
#include "AcopalypsPrototypeGameModeBase.h"

// Sets default values
AEnemyAICharacter::AEnemyAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set mesh to enemy mesh, and sets collision presets
	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(RootComponent);
	CharacterMesh->SetCollisionProfileName("Enemy");

	// Sets mesh for gun to socket on Character Mesh, and sets collision presets
	/*
	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("GunSocket"));
	Gun->SetOwner(this);
	*/

	Health = MaxHealth;
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
	DamageApplied = FMath::Min(Health, DamageApplied);
	Health -= DamageApplied;
	UE_LOG(LogTemp, Display, TEXT("health: %f"), Health);

	
	if(IsDead())
	{
		if (AAcopalypsPrototypeGameModeBase* PrototypeGameModeBase = GetWorld()->GetAuthGameMode<AAcopalypsPrototypeGameModeBase>())
		{
			PrototypeGameModeBase->PawnKilled(this);
		}
		DetachFromControllerPendingDestroy();
		CharacterMesh->SetCollisionProfileName("NoCollision");
		GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT(" Died: %s "), *GetName()));
	}
	return DamageApplied;
}

bool AEnemyAICharacter::IsDead() const
{
	return Health <= 0;
}

void AEnemyAICharacter::Shoot()
{
	GEngine->AddOnScreenDebugMessage(-1,2.f, FColor::Red, FString::Printf(TEXT(" Enemy Shooting")));
}
float AEnemyAICharacter::GetHealthPercent() const
{
	return Health/MaxHealth;
}