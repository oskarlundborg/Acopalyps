// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "AcopalypsCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Animation/AnimInstance.h"
#include "ExplosiveProjectile.h"
#include "FlareProjectile.h"
#include "BouncingProjectile.h"
#include "Projectile.h"
#include "BeanBagProjectile.h"
#include "Kismet/KismetMathLibrary.h"

AGun::AGun()
{
	SetAmmoRegular();
	SetAmmoExplosive();
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
}

/** Fire standard barrel of the gun */
void AGun::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	
	if(!bCanReload)
	{
		return;
	}
	
	FHitResult Hit;
	FVector ShotDirection;
	switch (CurrentAmmoType)
	{
	case Regular:
		if( CurrentMag > 0 )
		{
			FireRegular();
			FireTriggerEvent(Hit, ShotDirection, Regular);
		}
		break;
	case Bouncing:
		if( CurrentMag > 0 )
		{
			FireBouncing();
			FireTriggerEvent(Hit, ShotDirection, Bouncing);
		}
		break;
	default:break;
	}
}

/** Fire alternate barrel of the gun */
void AGun::AlternateFire()
{
	
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	if(!bCanReload)
	{
		return;
	}
	FHitResult Hit;
	FVector ShotDirection;
	switch (CurrentAlternateAmmoType)
	{
	case Explosive:
		if( CurrentMag > 0 )
		{
			FireExplosive();
			FireTriggerEvent(Hit, ShotDirection, Explosive);
		}
		break;
	case Flare:
		if( CurrentMag > 0 )
		{
			FireFlare();
			FireTriggerEvent(Hit, ShotDirection, Flare);
		}
		break;
	case BeanBag:
		if( CurrentMag > 0 )
		{
			FireBeanBag();
			FireTriggerEvent(Hit, ShotDirection, BeanBag);
		}
		break;
	default:break;
	}
}

/** Rapid Fire through regular barrel.*/
void AGun::RapidFire()
{
	if(!bCanReload)
	{
		return;
	}
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	if(CurrentAmmoType != AMMO_TYPES::Rapid)
	{
		return;
	}
	
	FHitResult Hit;
	FVector ShotDirection;
	if(CurrentMag > 0)
	{
		FireRapid();
		FireTriggerEvent(Hit, ShotDirection, Rapid);
	}
}

/** Snap weapon to player character 0 */
void AGun::AttachWeaponInputs(AAcopalypsCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}
	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}
		
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AGun::Fire);
			EnhancedInputComponent->BindAction(RapidFireAction, ETriggerEvent::Triggered, this, &AGun::RapidFire);
			EnhancedInputComponent->BindAction(AlternativeFireAction, ETriggerEvent::Triggered, this, &AGun::AlternateFire);
			// Reload
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AGun::AttemptReload);
			// Change ammo type
			EnhancedInputComponent->BindAction(ChangeAmmoRegularAction, ETriggerEvent::Triggered, this, &AGun::SetAmmoRegular);
			EnhancedInputComponent->BindAction(ChangeAmmoExplosiveAction, ETriggerEvent::Triggered, this, &AGun::SetAmmoExplosive);
			EnhancedInputComponent->BindAction(ChangeAmmoFlareAction, ETriggerEvent::Triggered, this, &AGun::SetAmmoFlare);
			EnhancedInputComponent->BindAction(ChangeAmmoBouncingAction, ETriggerEvent::Triggered, this, &AGun::SetAmmoBouncing);
			EnhancedInputComponent->BindAction(ChangeAmmoRapidAction, ETriggerEvent::Triggered, this, &AGun::SetAmmoRapid);
			EnhancedInputComponent->BindAction(ChangeAmmoBeanBagAction, ETriggerEvent::Triggered, this, &AGun::SetAmmoBeanBag);
		}
	}
}

AController* AGun::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(Character);
	if(OwnerPawn == nullptr)
	{
		return nullptr;
	}
	return OwnerPawn->GetController();
}

void AGun::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void AGun::Reload()
{
	ReloadCompletedEvent();
	if(AmmoCapacity > MaxMagSize-CurrentMag)
	{
		AmmoCapacity-=MaxMagSize-CurrentMag;
		CurrentMag = MaxMagSize;
	}
	else
	{
		CurrentMag+=AmmoCapacity;
		AmmoCapacity=0;
	}
	
	bCanReload=true;
}

/** Performs a ray casts, returns true if hit is registered */
bool AGun::GunTrace(FHitResult& HitResult, FVector& ShotDirection)
{
	AController* OwnerController = GetOwnerController();
	if( OwnerController == nullptr )
	{
		return false;
	}
	
	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	ShotDirection = Rotation.Vector();
	FVector End = Location + Rotation.Vector() * MaxRange;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Character);
	return GetWorld()->LineTraceSingleByChannel(HitResult, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);
}

void AGun::FireEnemy()
{
	CurrentMag-=*AmmoTypeCostValues.Find(CurrentAmmoType);
	if(CurrentMag<0) CurrentMag=0;
	if( RegularProjectileClass != nullptr )
	{
		const FRotator SpawnRotation = GetOwner()->GetActorForwardVector().Rotation();
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		FHitResult Hit;
		FVector ShotDirection;
		GunTrace(Hit, ShotDirection);

		GetWorld()->SpawnActor<AProjectile>(RegularProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParameters);
	}
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

// Fire actions per ammo type
void AGun::FireRegular()
{
	CurrentMag-=*AmmoTypeCostValues.Find(CurrentAmmoType);
	if(CurrentMag<0) CurrentMag=0;
	if( RegularProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());

		FHitResult Hit;
		FVector ShotDirection;
		
		FRotator SpawnRotation;
		if( GunTrace(Hit, ShotDirection) )
		{
			 SpawnRotation = UKismetMathLibrary::FindLookAtRotation(
				GetActorLocation() + GetActorRotation().RotateVector(FVector(-10, 0, 8)),
				Hit.Location
				);
		} else
		{
			SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		}
		
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParameters.Owner = this;

		GetWorld()->SpawnActor<AProjectile>(RegularProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParameters);
	}
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AGun::FireExplosive()
{
	CurrentMag-=*AmmoTypeCostValues.Find(CurrentAlternateAmmoType);
	if(CurrentMag<0) CurrentMag=0;
	if( ExplosiveProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		FHitResult Hit;
		FVector ShotDirection;
		
		FRotator SpawnRotation;
		if( GunTrace(Hit, ShotDirection) )
		{
			 SpawnRotation = UKismetMathLibrary::FindLookAtRotation(
				GetActorLocation() + GetActorRotation().RotateVector(FVector(-10, 0, 8)),
				Hit.Location
				);
		} else
		{
			SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		}
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		GetWorld()->SpawnActor<AExplosiveProjectile>(ExplosiveProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParameters);
	}

	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AGun::FireFlare()
{
	CurrentMag-=*AmmoTypeCostValues.Find(CurrentAlternateAmmoType);
	if(CurrentMag<0) CurrentMag=0;
	if( FlareProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		FHitResult Hit;
		FVector ShotDirection;
		
		FRotator SpawnRotation;
		if( GunTrace(Hit, ShotDirection) )
		{
			 SpawnRotation = UKismetMathLibrary::FindLookAtRotation(
				GetActorLocation() + GetActorRotation().RotateVector(FVector(-10, 0, 8)),
				Hit.Location
				);
		} else
		{
			SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		}
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		GetWorld()->SpawnActor<AFlareProjectile>(FlareProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParameters);
	}

	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AGun::FireBouncing()
{
	CurrentMag-=*AmmoTypeCostValues.Find(CurrentAmmoType);
	if(CurrentMag<0) CurrentMag=0;
	if( BouncingProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		FHitResult Hit;
		FVector ShotDirection;
		
		FRotator SpawnRotation;
		if( GunTrace(Hit, ShotDirection) )
		{
			 SpawnRotation = UKismetMathLibrary::FindLookAtRotation(
				GetActorLocation() + GetActorRotation().RotateVector(FVector(-10, 0, 8)),
				Hit.Location
				);
		} else
		{
			SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		}
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		GetWorld()->SpawnActor<ABouncingProjectile>(BouncingProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParameters);
	}
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AGun::FireBeanBag()
{
	CurrentMag-=*AmmoTypeCostValues.Find(CurrentAlternateAmmoType);
	if(CurrentMag<0) CurrentMag=0;
	if( BeanBagProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		FHitResult Hit;
		FVector ShotDirection;
		
		FRotator SpawnRotation;
		if( GunTrace(Hit, ShotDirection) )
		{
			 SpawnRotation = UKismetMathLibrary::FindLookAtRotation(
				GetActorLocation() + GetActorRotation().RotateVector(FVector(-10, 0, 8)),
				Hit.Location
				);
		} else
		{
			SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		}
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		GetWorld()->SpawnActor<ABeanBagProjectile>(BeanBagProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParameters);
	}

	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AGun::FireRapid()
{
	// Decrease ammo
	CurrentMag-=*AmmoTypeCostValues.Find(CurrentAmmoType);
	if(CurrentMag<0) CurrentMag=0;
	if( RegularProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		FHitResult Hit;
		FVector ShotDirection;
		
		FRotator SpawnRotation;
		if( GunTrace(Hit, ShotDirection) )
		{
			 SpawnRotation = UKismetMathLibrary::FindLookAtRotation(
				GetActorLocation() + GetActorRotation().RotateVector(FVector(-10, 0, 8)),
				Hit.Location
				);
		} else
		{
			SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		}
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
		//Randomize the spawnrotation to act as inaccuracy
		SpawnRotation = RandomRotator(SpawnRotation.Pitch,SpawnRotation.Yaw,SpawnRotation.Roll,InaccuracyModifier);

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		GetWorld()->SpawnActor<AProjectile>(RegularProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParameters);
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AGun::AttemptReload()
{
	if(bCanReload)
	{
		if(CurrentMag!=MaxMagSize)
		{
			ReloadTriggerEvent();
			bCanReload=false;
			GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AGun::Reload,ReloadTime );
		}
	}
}

FRotator AGun::RandomRotator(float Pitch, float Yaw, float Roll, float Interval) const
{
	const float NewPitch = FMath::FRandRange(Pitch-Interval,Pitch+Interval);
	const float NewYaw = FMath::FRandRange(Yaw-Interval,Yaw+Interval);
	return FRotator(NewPitch,NewYaw,Roll);
}

// Set Ammo Type
void AGun::SetAmmoRegular()
{
	CurrentAmmoType=Regular;
}
void AGun::SetAmmoExplosive()
{
	CurrentAlternateAmmoType=Explosive;
}
void AGun::SetAmmoFlare()
{
	CurrentAlternateAmmoType=Flare;
}
void AGun::SetAmmoBouncing()
{
	CurrentAmmoType=Bouncing;
}
void AGun::SetAmmoRapid()
{
	CurrentAmmoType=Rapid;
}
void AGun::SetAmmoBeanBag()
{
	CurrentAlternateAmmoType=BeanBag;
}

// Getters for equipped ammo
AMMO_TYPES AGun::GetCurrentAmmoType()
{
	return CurrentAmmoType;
}
AMMO_TYPES AGun::GetCurrentAlternateAmmoType()
{
	return CurrentAlternateAmmoType;
}






