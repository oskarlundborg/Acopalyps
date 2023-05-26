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
#include "LevelSpawner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AcopalypsPlatformGameInstance.h"
#include "SerializationSaveGame.h"
#include "Kismet/GameplayStatics.h" 

//////////////////////////////////////////////////////////////////////////
// AAcopalypsCharacter

AAcopalypsCharacter::AAcopalypsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->MaxWalkSpeed = WalkingMovementSpeed;
	
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
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
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
	
	if( GunClass != nullptr )
	{
		Gun = GetWorld()->SpawnActor<AGun>(GunClass);
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		Gun->AttachToComponent(
			Mesh1P,
			AttachmentRules,
			FName(TEXT("GripPoint"))
			);
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
	if( bIsCrouching && CurrentCapsuleHeight > CrouchHeight )
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), 24, CrouchInterpTime));
	} else if ( !bIsCrouching )
	{
		if( CurrentCapsuleHeight < 96 )
		{
			GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), 122, CrouchInterpTime));
			if(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() > 96) GetCapsuleComponent()->SetCapsuleHalfHeight(96);
		}
	}
	if( bRequestStopCrouching )
	{
		EndCrouch();
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
		//Crouching
		EnhancedInputComponent->BindAction<AAcopalypsCharacter, bool>(CrouchActionSlowMotion, ETriggerEvent::Started, this, &AAcopalypsCharacter::StartCrouch, true);
		EnhancedInputComponent->BindAction<AAcopalypsCharacter, bool>(CrouchAction, ETriggerEvent::Started, this, &AAcopalypsCharacter::StartCrouch, false);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AAcopalypsCharacter::EndCrouch);
		EnhancedInputComponent->BindAction(CrouchActionSlowMotion, ETriggerEvent::Completed, this, &AAcopalypsCharacter::EndCrouch);
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAcopalypsCharacter::Look);
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
	EnableInput(Cast<APlayerController>(GetController()));
	bIsDead = false;
	Load();
	SpawnTriggerEvent();
}

void AAcopalypsCharacter::ResetLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AAcopalypsCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AAcopalypsCharacter::SlowDownTime()
{
	SlowTimeTriggerEvent();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.4);
	GetWorldTimerManager().SetTimer(TimeTimerHandle, this, &AAcopalypsCharacter::ResumeTime, SlideTime, false);
	CustomTimeDilation = 1.6f;
	Gun->CustomTimeDilation = 1.6f;
}

void AAcopalypsCharacter::ResumeTime()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	CustomTimeDilation = 1.f;
	Gun->CustomTimeDilation = 1.f;
	ResumeTimeTriggerEvent();
}

void AAcopalypsCharacter::StartCrouch(bool SlowMotion)
{
	if( !bIsSliding )
	{
		StartSlide(SlowMotion);
	}
	CrouchTriggerEvent();
	bRequestStopCrouching = false;
	bIsCrouching = true;
	CharacterMovementComponent->MaxWalkSpeed = CrouchMovementSpeed;
}

void AAcopalypsCharacter::EndCrouch()
{
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActors(Children);
	Params.AddIgnoredActors(Gun->Children);
	const bool bHasCol = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		GetActorLocation() + FVector(0, 0, 98),
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeCapsule(55.f, 48.f),
		Params
		);
	bRequestStopCrouching = true;
	if( bIsSliding || bHasCol )
	{
		return;
	}
	CharacterMovementComponent->BrakingFrictionFactor = 2.f;
	CharacterMovementComponent->BrakingDecelerationWalking = 0.f;
	bIsCrouching = false;
	CharacterMovementComponent->MaxWalkSpeed = WalkingMovementSpeed;
}

void AAcopalypsCharacter::StartSlide(bool SlowMotion)
{
	bIsSliding = true;
	SlideTriggerEvent();
	CharacterMovementComponent->BrakingFrictionFactor = 0.075f;
	CharacterMovementComponent->BrakingDecelerationWalking = 0.f;
	if(  CharacterMovementComponent->IsMovingOnGround() )
	{
		if( CharacterMovementComponent->GetAnalogInputModifier() == 0 )
		{
			LaunchCharacter(
				GetActorForwardVector() * SlideStrength,
				false,
				false
				);
		} else
		{
			LaunchCharacter(
				CharacterMovementComponent->GetLastInputVector().GetSafeNormal() * SlideStrength,
				true,
				false
				);
		}
	}
	if(SlowMotion)
	{
		SlowDownTime();
	}
	GetWorldTimerManager().SetTimer(SlideHandle, this, &AAcopalypsCharacter::EndSlide, SlideTime, false);
}

void AAcopalypsCharacter::EndSlide()
{
	bIsSliding = false;
	CharacterMovementComponent->BrakingFrictionFactor = .3f;
	CharacterMovementComponent->BrakingDecelerationWalking = 2048.f;
	if( bRequestStopCrouching )
	{
		EndCrouch();
	}
}

void AAcopalypsCharacter::Jump()
{
	Super::Jump();
}

void AAcopalypsCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * MouseSensitivity);
		AddControllerPitchInput(LookAxisVector.Y * MouseSensitivity);
	}
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
			GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AAcopalypsCharacter::Respawn, 1);
			GEngine->AddOnScreenDebugMessage(-1,6.f, FColor::Yellow, FString::Printf(TEXT(" Died: %s "), *GetName()));
			bIsDead = true;
		}
	return DamageApplied;
	}
	return 0;
}

void AAcopalypsCharacter::Save()
{
	//TArray<AActor*> AllActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
	//Cast<UAcopalypsPlatformGameInstance>(GetGameInstance())->SaveGame(AllActors);
	if( USerializationSaveGame* Save = Cast<USerializationSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass)) )
	{
		//TArray<AActor*> AllActors;
		//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
		//SaveGame->SaveGameInstance(GetWorld(), AllActors);
		Save->Save(this);
		if( IsValid(Save) )
		{
			//UGameplayStatics::DeleteGameInSlot(TEXT("default"), 0);
			UGameplayStatics::SaveGameToSlot(Save, TEXT("default"), 0);
		}
	} else { GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::Red, TEXT("Error: Unable to save...")); }
}

void AAcopalypsCharacter::Load()
{
	//Cast<UAcopalypsPlatformGameInstance>(GetGameInstance())->LoadGame();
	if( USerializationSaveGame* Save = Cast<USerializationSaveGame>(UGameplayStatics::LoadGameFromSlot("default", 0)) ) {
		//TArray<AActor*> AllActors;
		//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
		//SaveGame->LoadGameInstance(GetWorld(), AllActors);
		Save->Load(this);
	} else { GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::Red, TEXT("Error: No valid save to load...")); }
}

void AAcopalypsCharacter::SetLoadedLevels(TArray<FLevelID> LevelsToLoad)
{
	LoadedLevels.Empty();
	LoadedLevels = LevelsToLoad;
}
