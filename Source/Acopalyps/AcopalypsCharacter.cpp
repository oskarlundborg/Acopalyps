// Copyright Epic Games, Inc. All Rights Reserved.

#include "AcopalypsCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AcopalypsPrototypeGameModeBase.h"
#include "AcopalypsSaveGame.h"
#include "EnemyAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h" 
#include "Kismet/KismetMathLibrary.h"

//////////////////////////////////////////////////////////////////////////
// AAcopalypsCharacter

AAcopalypsCharacter::AAcopalypsCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	CharacterMovementComponent = GetCharacterMovement();
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetMaxHealth(1000);
		
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
	
	if( GunClass != nullptr )
	{
		Gun = GetWorld()->SpawnActor<AGun>(GunClass);
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		Gun->AttachToComponent(
			Mesh1P,
			AttachmentRules,
			FName(TEXT("GripPoint"))
			);
		SetHasRifle(true);
		Gun->SetOwner(this);
		Gun->AttachWeaponInputs(this);
	}
	SpawnPosition = GetActorLocation();
}

void AAcopalypsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaTime);
	float const CurrentCapsuleHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	if( bIsCrouching && CurrentCapsuleHeight > 56 )
	{
		if( CharacterMovementComponent->Velocity.Size() > 400 )
		{
			CharacterMovementComponent->GroundFriction = 0.f;
			if( CharacterMovementComponent->Velocity.Size() > 800 )
			{
				Kick();
			}
		}
		else
		{
			CharacterMovementComponent->GroundFriction = 8.f;
		}
		GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), 32, CrouchInterpTime));
	} else if ( !bIsCrouching )
	{
		if( CurrentCapsuleHeight < 98 )
		{
			GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), 122, CrouchInterpTime));
		}
		if ( CharacterMovementComponent->GroundFriction < 8.f )
		{
			CharacterMovementComponent->GroundFriction += 0.1f;
		}
	}
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
		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AAcopalypsCharacter::EndSprint);
		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AAcopalypsCharacter::EndCrouch);
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::Look);
		//Kicking
		EnhancedInputComponent->BindAction(KickAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::Kick);
		//Slow Down Time
		EnhancedInputComponent->BindAction(SlowDownTimeAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::SlowDownTime);
		//Respawn
		EnhancedInputComponent->BindAction(RespawnAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::Respawn);
		//Reset Level
		EnhancedInputComponent->BindAction(ResetLevelAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::ResetLevel);
		//Save & Load
		EnhancedInputComponent->BindAction(SaveAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::Save);
		EnhancedInputComponent->BindAction(LoadAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::Load);
	}
}

void AAcopalypsCharacter::Respawn()
{
	UE_LOG(LogTemp, Display, TEXT("Calling respawn"))
	//Cast<UAcopalypsPlatformGameInstance>(GetWorld()->GetGameInstance())->LoadGame();
	UGameplayStatics::GetGameMode(this)->RestartPlayer(GetController());
	HealthComponent->RefillHealth();
	SetActorLocation(SpawnPosition);
	EnableInput(Cast<APlayerController>(GetController()));
	bIsDead = false;
	SpawnTriggerEvent();
}

void AAcopalypsCharacter::ResetLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
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

void AAcopalypsCharacter::SlowDownTime()
{
	SlowTimeTriggerEvent();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.4);
	GetWorldTimerManager().SetTimer(TimeTimerHandle, this, &AAcopalypsCharacter::ResumeTime, 2.f, false);
	CustomTimeDilation = 1.6f;
}

void AAcopalypsCharacter::ResumeTime()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	CustomTimeDilation = 1.f;
	ResumeTimeTriggerEvent();
}

void AAcopalypsCharacter::StartCrouch()
{
	if( CharacterMovementComponent->IsMovingOnGround() )
	{
		LaunchCharacter(
			CharacterMovementComponent->Velocity / 8,
			false,
			false
			);
	}
	CrouchTriggerEvent();
	bIsCrouching = true;
	CharacterMovementComponent->MaxWalkSpeed = CrouchMovementSpeed;
}

void AAcopalypsCharacter::EndCrouch()
{
	bIsCrouching = false;
	CharacterMovementComponent->MaxWalkSpeed = WalkingMovementSpeed;
}

void AAcopalypsCharacter::StartSprint()
{
	if( !bIsCrouching )
	{
		bIsSprinting = true;
		SprintTriggerEvent();
		CharacterMovementComponent->MaxWalkSpeed = SprintMovementSpeed;
	}
}

void AAcopalypsCharacter::EndSprint()
{
	bIsSprinting = false;
	if( !bIsCrouching )
	{
		CharacterMovementComponent->MaxWalkSpeed = WalkingMovementSpeed;
	}
}

void AAcopalypsCharacter::Jump()
{
	Super::Jump();
	HideLeg();
}

void AAcopalypsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X * MouseSensitivity);
		AddControllerPitchInput(LookAxisVector.Y * MouseSensitivity);
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
			Enemy->RagDoll(GetActorForwardVector() * 50000 + FVector(0, 0, 4000));
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
			Enemy->RagDoll(GetActorForwardVector() * 50000 + FVector(0, 0, 4000));
			//Enemy->GetMesh()->AddForce(GetActorForwardVector() * 1000);
		}
	}
	KickTriggerEvent(SweepResult);
}

float AAcopalypsCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if( !bIsDead )
	{
		float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		DamageApplied = FMath::Min(HealthComponent->GetHealth(), DamageApplied);
		HealthComponent->SetHealth(HealthComponent->GetHealth() - DamageApplied);
		UE_LOG(LogTemp, Display, TEXT("health: %f"), HealthComponent->GetHealth());

		const AActor* ConstDamageCauser = DamageCauser;
		TakeDamageTriggerEvent(DamageAmount, ConstDamageCauser);
	
		if( HealthComponent->IsDead() )
		{
			if (AAcopalypsPrototypeGameModeBase* PrototypeGameModeBase = GetWorld()->GetAuthGameMode<AAcopalypsPrototypeGameModeBase>())
			{
				PrototypeGameModeBase->PawnKilled(this);
			}
			//DetachFromControllerPendingDestroy();
			DisableInput(Cast<APlayerController>(GetController()));
			//GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
			HideLeg();
			GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AAcopalypsCharacter::Respawn, 1);
			GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT(" Died: %s "), *GetName()));
			bIsDead = true;
		}
	return DamageApplied;
	}
	return 0;
}

void AAcopalypsCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

void AAcopalypsCharacter::Save()
{
	UAcopalypsSaveGame* SaveGame = Cast<UAcopalypsSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
	SaveGame->SaveGameInstance(GetWorld(), AllActors);
	UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("default"), 0);
	//UE_LOG(LogTemp, Display, TEXT("%s"), *SaveGame->PlayerInstance.Transform.ToString())
}

void AAcopalypsCharacter::Load()
{
	UAcopalypsSaveGame* SaveGame = Cast<UAcopalypsSaveGame>(UGameplayStatics::LoadGameFromSlot("default", 0));
	if( SaveGame != nullptr ) {
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
		SaveGame->LoadGameInstance(GetWorld(), AllActors);
	} else { GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red, TEXT("No Game To Load...")); }
}
