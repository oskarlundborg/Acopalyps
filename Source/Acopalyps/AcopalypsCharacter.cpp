// Copyright Epic Games, Inc. All Rights Reserved.

#include "AcopalypsCharacter.h"
#include "AcopalypsProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/BoxComponent.h"
#include "AcopalypsPrototypeGameModeBase.h"
#include "EnemyAICharacter.h"


//////////////////////////////////////////////////////////////////////////
// AAcopalypsCharacter

AAcopalypsCharacter::AAcopalypsCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));


	// Create a mesh component that will be used to kick
	LegMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Leg"));
	LegMesh->SetupAttachment(GetCapsuleComponent());
}

void AAcopalypsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//      Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	// method call to hide and disable leg mesh and leg hitbox
	HideLeg();
	
	// Sets methods to be run when LegCollision hits enemies
	//LegMesh->OnComponentHit.AddDynamic(this, &AAcopalypsCharacter::OnKickAttackHit);
	LegMesh->OnComponentBeginOverlap.AddDynamic(this, &AAcopalypsCharacter::OnKickAttackOverlap);

	Health = MaxHealth;
}

//////////////////////////////////////////////////////////////////////////// Input

void AAcopalypsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::Look);

		//Kicking
		EnhancedInputComponent->BindAction(KickAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::Kick);
	}
}


void AAcopalypsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AAcopalypsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AAcopalypsCharacter::Kick()
{
	if (Controller != nullptr)
	{
		// makes leg mesh visible, and sets collision response of the leg mesh and box collider to collide
		LegMesh->SetVisibility(true);
		LegMesh->SetCollisionProfileName("Weapon");
		LegMesh->SetNotifyRigidBodyCollision(true);
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AAcopalypsCharacter::HideLeg, 1.f, false);
	}
}

void AAcopalypsCharacter::HideLeg() const
{
	// makes leg mesh unvisible, and sets collision response to none on leg mesh and boxCollision-object
	LegMesh->SetVisibility(false);
	LegMesh->SetCollisionProfileName("NoCollision");
	LegMesh->SetNotifyRigidBodyCollision(false);
	
}

void AAcopalypsCharacter::OnKickAttackHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT("hit between: %s %s"), *HitComponent->GetName(), *OtherComp->GetName()));
	if(OtherActor->ActorHasTag(TEXT("Enemy"))) {

		GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT("hit between: %s %s"), *HitComponent->GetName(), *OtherComp->GetName()));

		AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(OtherActor);
		if(Enemy)
		{
			GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT("hit between: %s %s"), *HitComponent->GetName(), *OtherComp->GetName()));
			Enemy->RagDoll();
			Enemy->GetMesh()->AddForce(GetActorForwardVector() * 1000);
		}
	}
}

void AAcopalypsCharacter::OnKickAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT("hit between: %s %s"), *OverlappedComponent->GetName(),*OtherComp->GetName()));
	if(OtherActor->ActorHasTag(TEXT("Enemy"))) {

		GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT("hit between: %s %s"), *OverlappedComponent->GetName(),*OtherComp->GetName()));

		AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(OtherActor);
		if(Enemy)
		{
			GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT("hit between: %s %s"), *OverlappedComponent->GetName(),*OtherComp->GetName()));
			Enemy->RagDoll();
			Enemy->GetMesh()->AddForce(GetActorForwardVector() * 1000);
		}
	}
}

float AAcopalypsCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
		GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
		HideLeg();
		GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT(" Died: %s "), *GetName()));
	}
	return DamageApplied;
}

bool AAcopalypsCharacter::IsDead() const
{
	return Health <= 0;
}

float AAcopalypsCharacter::GetHealthPercent() const
{
	return Health/MaxHealth;
}
void AAcopalypsCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AAcopalypsCharacter::GetHasRifle()
{
	return bHasRifle;
}