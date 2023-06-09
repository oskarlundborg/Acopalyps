// Fill out your copyright notice in the Description page of Project Settings.
/**
 @author: Gin Lindelöw
 @author: Joakim Pettersson
*/

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
	FProjectileInfo* Projectile = Projectiles.Find(AmmoType);
	if(CurrentMag==0)
	{
		EmptyMagFireAttemptEvent();
	}
	if( Projectile->Class != nullptr && (CurrentMag > 0 || Projectile->Cost == 0) && bCanReload && GetOwner() != nullptr )
	{
		CurrentMag -= Projectile->Cost;
		if(CurrentMag < 0)
		{
			CurrentMag = 0;
		}
		FHitResult Hit;
		FVector ShotDirection;
		FRotator SpawnRotation;
		int8 ProjectilesToSpawn = 1;
		if( HitTrace(Hit, ShotDirection) )
		{
			SpawnRotation = UKismetMathLibrary::FindLookAtRotation(
			   GetActorLocation() + GetActorRotation().RotateVector(FVector(-10, 0, 8)),
			   Hit.Location
			   );
		} else
		{
			if( GetOwner()->ActorHasTag("Player") && GetOwnerController() != nullptr )
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
		if(AmmoType == Shotgun)
		{
			ProjectilesToSpawn = 8;
		}
		FActorSpawnParameters ActorSpawnParameters;
		
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ActorSpawnParameters.Owner = this;

		for(int i = 0; i<ProjectilesToSpawn;i++)
		{
			if(AmmoType == Shotgun)
			{
				SpawnRotation = RandomRotator(SpawnRotation.Pitch,SpawnRotation.Yaw,SpawnRotation.Roll,ShotgunSpread);
			}
			GetWorld()->SpawnActor<AProjectile>(
				Projectile->Class,
				GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset),
				SpawnRotation,
				ActorSpawnParameters
				)->SetOwner(this);
		}
		
		FireTriggerEvent(Hit, ShotDirection, AmmoType);
	}
}

void AGun::PrimaryFire()
{
	const AMMO_TYPES AmmoToFire = CurrentAmmoType; // For TimerHandle to handle same ammo when switching mid delay
	UE_LOG(LogTemp, Display, TEXT("Ammo: %d, CanFire: %i"), AmmoToFire, Projectiles.Find(AmmoToFire)->bCanFire)
	if( Projectiles.Find(AmmoToFire)->bCanFire && bCanReload)
	{
		Fire(AmmoToFire);
		ToggleCanFirePrimary(AmmoToFire);
		CanFirePrimaryDelegate.BindUFunction(this, FName("ToggleCanFirePrimary"), AmmoToFire);
		GetWorldTimerManager().SetTimer(
			Projectiles.Find(AmmoToFire)->TimerHandle,
			CanFirePrimaryDelegate,
			Projectiles.Find(AmmoToFire)->Delay,
			false
			);
	}
}

void AGun::AlternateFire()
{
	const AMMO_TYPES AmmoToFire = CurrentAlternateAmmoType; // For TimerHandle to handle same ammo when switching mid delay
	UE_LOG(LogTemp, Display, TEXT("Ammo: %d, CanFire: %i"), AmmoToFire, Projectiles.Find(AmmoToFire)->bCanFire)
	if( Projectiles.Find(AmmoToFire)->bCanFire && bCanReload)
	{
		Fire(AmmoToFire);
		ToggleCanFireAlternate(AmmoToFire);
		CanFireAlternateDelegate.BindUFunction(this, FName("ToggleCanFireAlternate"), AmmoToFire);
		GetWorldTimerManager().SetTimer(
			Projectiles.Find(AmmoToFire)->TimerHandle,
			CanFireAlternateDelegate,
			Projectiles.Find(AmmoToFire)->Delay,
			false
			);
	}
}

void AGun::FlareFire()
{
	const AMMO_TYPES AmmoToFire = Flare;
	UE_LOG(LogTemp, Display, TEXT("Ammo: %d, CanFire: %i"), AmmoToFire, Projectiles.Find(AmmoToFire)->bCanFire)
	if( Projectiles.Find(AmmoToFire)->bCanFire && bCanReload)
	{
		Fire(AmmoToFire);
		ToggleCanFireAlternate(AmmoToFire);
		CanFireAlternateDelegate.BindUFunction(this, FName("ToggleCanFireAlternate"), AmmoToFire);
		GetWorldTimerManager().SetTimer(
			Projectiles.Find(AmmoToFire)->TimerHandle,
			CanFireAlternateDelegate,
			Projectiles.Find(AmmoToFire)->Delay,
			false
			);
	}
}

/** Performs a ray casts, returns true if hit is registered */
bool AGun::HitTrace(FHitResult& HitResult, FVector& ShotDirection)
{
	const AController* OwnerController = GetOwnerController();
	if( OwnerController == nullptr )
	{
		return false;
	}
	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	ShotDirection = Rotation.Vector();
	const FVector End = Location + Rotation.Vector() * MaxRange;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Character);
	return GetWorld()->LineTraceSingleByChannel(HitResult, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);
}

/** Adds gun inputs to character */
void AGun::AttachWeaponInputs(ACharacter* TargetCharacter)
{
	if( (Character = TargetCharacter) == nullptr )
	{
		return;
	}
	// Set up action bindings
	if( const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()) )
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
			EnhancedInputComponent->BindAction(FlareFireAction, ETriggerEvent::Triggered, this, &AGun::FlareFire);
			// Reload
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AGun::AttemptReload);
			// Change ammo type
			
			EnhancedInputComponent->BindAction<AGun>(RotateRegularAmmoForward, ETriggerEvent::Started, this, &AGun::RotateAmmoTypeForward);
			EnhancedInputComponent->BindAction<AGun>(RotateRegularAmmoBackward, ETriggerEvent::Started, this, &AGun::RotateAmmoTypeBackward);
			EnhancedInputComponent->BindAction<AGun>(RotateAlternateAmmo, ETriggerEvent::Started, this, &AGun::RotateAlternateAmmoType);
			
			EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoRegularAction, ETriggerEvent::Started, this, &AGun::SetPrimaryAmmoType, Regular);
			EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoBouncingAction, ETriggerEvent::Started, this, &AGun::SetPrimaryAmmoType, Bouncing);
			EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoRapidAction, ETriggerEvent::Started, this, &AGun::SetPrimaryAmmoType, Rapid);
			EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoExplosiveAction, ETriggerEvent::Started, this, &AGun::SetAlternateAmmoType, Explosive);
			//EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoFlareAction, ETriggerEvent::Started, this, &AGun::SetAlternateAmmoType, Flare);
			EnhancedInputComponent->BindAction<AGun, TEnumAsByte<AMMO_TYPES>>(ChangeAmmoBeanBagAction, ETriggerEvent::Started, this, &AGun::SetAlternateAmmoType, Shotgun);
		}
	}
}

AController* AGun::GetOwnerController() const
{
	const APawn* OwnerPawn = Cast<APawn>(Character);
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
	if( const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()) )
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void AGun::Reload()
{
	if(bCanReload == false)
	{
		ReloadCompletedEvent();
		CurrentMag = MaxMagSize;
		bCanReload=true;
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

void AGun::SetPrimaryAmmoType(TEnumAsByte<AMMO_TYPES> AmmoType)
{
	SwapRegularAmmoTriggerEvent();
	CurrentAmmoType = AmmoType;
}

void AGun::SetAlternateAmmoType(TEnumAsByte<AMMO_TYPES> AmmoType)
{
	SwapAlternateAmmoTriggerEvent();
	CurrentAlternateAmmoType = AmmoType;
}

void AGun::RotateAmmoTypeForward()
{
	if(AmmoTypes.Find(CurrentAmmoType) == AmmoTypes.Num()-1)
	{
		SetPrimaryAmmoType(AmmoTypes[0]);
	}
	else
	{
		SetPrimaryAmmoType(AmmoTypes[(AmmoTypes.IndexOfByKey(CurrentAmmoType)+1)]);
	}
}

void AGun::RotateAmmoTypeBackward()
{
	if(AmmoTypes.Find(CurrentAmmoType) == AmmoTypes[0])
	{
		SetPrimaryAmmoType(AmmoTypes[AmmoTypes.Num()-1]);
	}
	else
	{
		SetPrimaryAmmoType(AmmoTypes[(AmmoTypes.IndexOfByKey(CurrentAmmoType)-1)]);
	}
}

void AGun::RotateAlternateAmmoType()
{
	if(AlternateAmmoTypes.Find(CurrentAlternateAmmoType) == AlternateAmmoTypes.Num()-1)
	{
		SetAlternateAmmoType(AlternateAmmoTypes[0]);
	}
	else
	{
		SetAlternateAmmoType(AlternateAmmoTypes[(AlternateAmmoTypes.IndexOfByKey(CurrentAlternateAmmoType)+1)]);
	}
}
