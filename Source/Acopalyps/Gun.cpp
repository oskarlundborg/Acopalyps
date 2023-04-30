// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "AcopalypsCharacter.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Animation/AnimInstance.h"
#include "Projectile.h"
#include "Kismet/KismetMathLibrary.h"

/** Fire standard barrel of the gun */
void AGun::Fire(TEnumAsByte<AMMO_TYPES> AmmoType)
{
	if( *Projectiles.Find(AmmoType)->Class != nullptr && CurrentMag >= Projectiles.Find(AmmoType)->Cost && bCanReload )
	{
		CurrentMag -= Projectiles.Find(AmmoType)->Cost;
		FHitResult Hit;
		FVector ShotDirection;
		FRotator SpawnRotation;
		if( HitTrace(Hit, ShotDirection) )
		{
			SpawnRotation = UKismetMathLibrary::FindLookAtRotation(
			   GetActorLocation() + GetActorRotation().RotateVector(FVector(-10, 0, 8)),
			   Hit.Location
			   );
		} else
		{
			if( GetOwner()->ActorHasTag("Player"))
			{
				SpawnRotation = Cast<APlayerController>(GetOwnerController())->PlayerCameraManager->GetCameraRotation();
			} else
			{
				SpawnRotation = GetOwner()->GetActorForwardVector().Rotation();
			}
		}
		if( AmmoType == Rapid )
		{
			SpawnRotation = RandomRotator(SpawnRotation.Pitch,SpawnRotation.Yaw,SpawnRotation.Roll,InaccuracyModifier);
		}
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParameters.Owner = this;

		GetWorld()->SpawnActor<AProjectile>(
			*Projectiles.Find(AmmoType)->Class,
			GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset),
			SpawnRotation,
			ActorSpawnParameters
			)->SetOwner(this);
		FireTriggerEvent(Hit, ShotDirection, AmmoType);
	}
}

void AGun::PrimaryFire()
{
	Fire(CurrentAmmoType);
}

void AGun::AlternateFire()
{
	Fire(CurrentAlternateAmmoType);
}

void AGun::RapidFire()
{
	if( CurrentAmmoType == Rapid )
	{
		Fire(Rapid);
	}
}

/** Performs a ray casts, returns true if hit is registered */
bool AGun::HitTrace(FHitResult& HitResult, FVector& ShotDirection)
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

/** Adds gun inputs to character */
void AGun::AttachWeaponInputs(ACharacter* TargetCharacter)
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
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AGun::PrimaryFire);
			EnhancedInputComponent->BindAction(AlternativeFireAction, ETriggerEvent::Started, this, &AGun::AlternateFire);
			EnhancedInputComponent->BindAction(RapidFireAction, ETriggerEvent::Triggered, this, &AGun::RapidFire);
			// Reload
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AGun::AttemptReload);
			// Change ammo type
			EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoRegularAction, ETriggerEvent::Started, this, &AGun::SetPrimaryAmmoType, Regular);
			EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoBouncingAction, ETriggerEvent::Started, this, &AGun::SetPrimaryAmmoType, Bouncing);
			EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoRapidAction, ETriggerEvent::Started, this, &AGun::SetPrimaryAmmoType, Rapid);
			EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoExplosiveAction, ETriggerEvent::Started, this, &AGun::SetAlternateAmmoType, Explosive);
			EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoFlareAction, ETriggerEvent::Started, this, &AGun::SetAlternateAmmoType, Flare);
			EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoBeanBagAction, ETriggerEvent::Started, this, &AGun::SetAlternateAmmoType, BeanBag);
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

void AGun::SetPrimaryAmmoType(TEnumAsByte<AMMO_TYPES> AmmoType)
{
	CurrentAmmoType = AmmoType;
}

void AGun::SetAlternateAmmoType(TEnumAsByte<AMMO_TYPES> AmmoType)
{
	CurrentAlternateAmmoType = AmmoType;
}
