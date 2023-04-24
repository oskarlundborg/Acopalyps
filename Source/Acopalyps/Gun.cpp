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

AGun::AGun()
{
	MuzzleOffset = FVector(100.0, 0.0, 10.0);
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
	
	FHitResult Hit;
	FVector ShotDirection;
	switch (CurrentAmmoType)
	{
	case Regular:
		if( RegularMag > 0 )
		{
			FireRegular();
			FireTriggerEvent(Hit, ShotDirection, Regular);
		}
		break;
	case Bouncing:
		if( BouncingMag > 0 )
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

	FHitResult Hit;
	FVector ShotDirection;
	switch (CurrentAlternateAmmoType)
	{
	case Explosive:
		if( ExplosiveMag > 0 )
		{
			FireExplosive();
			FireTriggerEvent(Hit, ShotDirection, Explosive);
			AlternateReload();
		}
		break;
	case Flare:
		if( FlareMag > 0 )
		{
			FireFlare();
			FireTriggerEvent(Hit, ShotDirection, Flare);
			AlternateReload();
		}
		break;
	case BeanBag:
		if( BeanBagMag > 0 )
		{
			FireBeanBag();
			FireTriggerEvent(Hit, ShotDirection, BeanBag);
			AlternateReload();
		}
		break;
	default:break;
	}
}

/** Rapid Fire through regular barrel.*/
void AGun::RapidFire()
{
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
	if(RapidMag > 0)
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
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AGun::Reload);
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
	ReloadTriggerEvent(CurrentAmmoType);
	
	int32 Total;
	switch (CurrentAmmoType)
	{
	case Regular:
		// Load regular ammo if has
		Total = *(Character->GetAmmoCountMap()->Find(Regular));
		if(Total == 0)
		{
			return;
		}
		if( Total + RegularMag <= 12 )
		{
			Character->GetAmmoCountMap()->Emplace(Regular, 0);
			SetRegularMag(Total+RegularMag);
		} else
		{
			Character->GetAmmoCountMap()->Emplace(Regular,(Total - 12) + RegularMag);
			SetRegularMag(12);
		}
		break;
	case Bouncing:
		Total = *(Character->GetAmmoCountMap()->Find(Bouncing));
		if(Total==0)
		{
			return;
		}
		if( Total + BouncingMag <= 12 )
		{
			Character->GetAmmoCountMap()->Emplace(Bouncing, 0);
			SetBouncingMag(Total+BouncingMag);
		} else
		{
			Character->GetAmmoCountMap()->Emplace(Bouncing,(Total - 12) + BouncingMag);
			SetBouncingMag(12);
		}
		break;
	case Rapid:
		//Load Rapid Fire ammo if available
		Total = *(Character->GetAmmoCountMap()->Find(Rapid));
		if(Total == 0)
		{
			//Ladda med 0 i reserv
			return;
		}
		
		if((Total+RapidMag)<=12)
		{
			Character->GetAmmoCountMap()->Emplace(Rapid, 0);
			SetRapidMag(Total + RapidMag);
		}

		else
		{
			Character->GetAmmoCountMap()->Emplace(Rapid, (Total-12) + RapidMag);
			SetRapidMag(12);
		}
		
		
	default:break;
	}
}

void AGun::AlternateReload()
{
	AlternateReloadTriggerEvent(CurrentAlternateAmmoType);

	// Load alternate ammo
	int32 Total;
	switch (CurrentAlternateAmmoType)
	{
	case Explosive:
		Total = *(Character->GetAmmoCountMap()->Find(Explosive));
		if( Total <= 1 )
		{
			Character->GetAmmoCountMap()->Emplace(Explosive,0);
			SetExplosiveMag(Total);
		} else
		{
			Character->GetAmmoCountMap()->Emplace(Explosive,Total - 1);
			SetExplosiveMag(1);
		}
		break;
	case Flare:
		Total = *(Character->GetAmmoCountMap()->Find(Flare));
		if( Total <= 1 )
		{
			Character->GetAmmoCountMap()->Emplace(Flare,0);
			SetFlareMag(Total);
		} else
		{
			Character->GetAmmoCountMap()->Emplace(Flare,Total - 1);
			SetFlareMag(1);
		}
		break;
	case BeanBag:
		Total = *(Character->GetAmmoCountMap()->Find(BeanBag));
		if( Total <= 1 )
		{
			Character->GetAmmoCountMap()->Emplace(BeanBag,0);
			SetBeanBagMag(Total);
		} else
		{
			Character->GetAmmoCountMap()->Emplace(BeanBag,Total - 1);
			SetBeanBagMag(1);
		}
		break;
	default:break;
	}
}

// Set Mag Size
void AGun::SetRegularMag(int32 Size)
{
	RegularMag = Size;
}
void AGun::SetBouncingMag(int32 Size)
{
	BouncingMag = Size;
}
void AGun::SetExplosiveMag(int32 Size)
{
	ExplosiveMag = Size;
}
void AGun::SetFlareMag(int32 Size)
{
	FlareMag = Size;
}
void AGun::SetRapidMag(int32 Size)
{
	RapidMag = Size;
}
void AGun::SetBeanBagMag(int32 Size)
{
	BeanBagMag = Size;
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

void AGun::FireEnemy()
{
	RegularMag--;
	if( RegularProjectileClass != nullptr )
	{
		const FRotator SpawnRotation = GetOwner()->GetActorForwardVector().Rotation();
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

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

// Fire actions per ammo type
void AGun::FireRegular()
{
	RegularMag--;
	if( RegularProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

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

void AGun::FireExplosive()
{
	ExplosiveMag--;
	if( ExplosiveProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
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
	FlareMag--;
	if( FlareProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
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
	BouncingMag--;
	if( BouncingProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
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
	BeanBagMag--;
	if( BeanBagProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
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
	RapidMag--;
	if( RegularProjectileClass != nullptr )
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
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

FRotator AGun::RandomRotator(float Pitch, float Yaw, float Roll, float Interval) const
{
	const float NewPitch = FMath::FRandRange(Pitch-Interval,Pitch+Interval);
	const float NewYaw = FMath::FRandRange(Yaw-Interval,Yaw+Interval);
	return FRotator(NewPitch,NewYaw,Roll);
}






