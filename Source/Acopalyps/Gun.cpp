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
void AGun::Fire()
{
	if( Character == nullptr || Character->GetController() == nullptr || !bCanReload )
	{
		return;
	}
	if( *Projectiles.Find(CurrentAmmoType)->Class != nullptr && CurrentMag >= Projectiles.Find(CurrentAmmoType)->Cost )
	{
		CurrentMag -= Projectiles.Find(CurrentAmmoType)->Cost;
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());

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
			SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		}
		if( CurrentAmmoType == Rapid )
		{
			SpawnRotation = RandomRotator(SpawnRotation.Pitch,SpawnRotation.Yaw,SpawnRotation.Roll,InaccuracyModifier);
		}
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParameters.Owner = this;

		GetWorld()->SpawnActor<AProjectile>(
			*Projectiles.Find(CurrentAmmoType)->Class,
			GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset),
			SpawnRotation,
			ActorSpawnParameters
			);

		FireTriggerEvent(Hit, ShotDirection, CurrentAmmoType);
	}
}

void AGun::RapidFire()
{
	if( CurrentAmmoType == Rapid )
	{
		Fire();
	}
}

/** Fire alternate barrel of the gun */
void AGun::AlternateFire()
{
	if( Character == nullptr || Character->GetController() == nullptr || !bCanReload )
	{
		return;
	}
	if( *Projectiles.Find(CurrentAlternateAmmoType)->Class != nullptr && CurrentMag >= Projectiles.Find(CurrentAlternateAmmoType)->Cost )
	{
		CurrentMag -= Projectiles.Find(CurrentAlternateAmmoType)->Cost;
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());

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
			SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		}
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParameters.Owner = this;

		GetWorld()->SpawnActor<AProjectile>(
			*Projectiles.Find(CurrentAlternateAmmoType)->Class,
			GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset),
			SpawnRotation,
			ActorSpawnParameters
			);

		FireTriggerEvent(Hit, ShotDirection, CurrentAlternateAmmoType);
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
