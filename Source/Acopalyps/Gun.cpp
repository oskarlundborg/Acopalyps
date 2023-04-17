// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "AcopalypsCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/DamageEvents.h"
#include "Engine/LocalPlayer.h"
#include "Animation/AnimInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "ExplosiveProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AGun::AGun()
{
	MuzzleOffset = FVector(100.0, 0.0, 10.0);
	SetAmmoRegular();
	SetAmmoExplosive();
}

void AGun::BeginPlay()
{
	Super::BeginPlay();

	//Root = CreateDefaultSubobject<USceneComponent>();
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
			FireRegular(Hit, ShotDirection);
		}
		break;
	case Piercing:
		if( PiercingMag > 0 )
		{
			FirePiercing(Hit, ShotDirection);
		}
		break;
	default:break;
	}
	AController* OwnerController = GetOwnerController();
	if(OwnerController == nullptr)
	{
		return;
	}
	//UStaticMeshComponent* MeshComponent  = Cast<UStaticMeshComponent>(Hit.GetActor()->GetRootComponent());
	//if(MeshComponent && Hit.GetActor()->IsRootComponentMovable())
	//{
	//	FVector Location;
	//	FRotator Rotation;
	//	OwnerController->GetPlayerViewPoint(Location, Rotation);
	//	ShotDirection = Rotation.Vector();
	//	MeshComponent->AddImpulse(ShotDirection * ImpulseForce * MeshComponent->GetMass());
	//}
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
			FireExplosive(Hit, ShotDirection);
			AlternateReload();
		}
		break;
	case Flare:
		if( FlareMag > 0 )
		{
			FireFlare(Hit, ShotDirection);
			AlternateReload();
		}
		break;
	default:break;
	}
}

/** Rapid Fire through regular barrel.*/
void AGun::RapidFire()
{
	UE_LOG(LogTemp,Display,TEXT("Trying to fire rapidly"));
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
		FireRapid(Hit, ShotDirection);
	}
	
	AController* OwnerController = GetOwnerController();
	if(OwnerController == nullptr)
	{
		return;
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
			EnhancedInputComponent->BindAction(ChangeAmmoPiercingAction, ETriggerEvent::Triggered, this, &AGun::SetAmmoPiercing);
			EnhancedInputComponent->BindAction(ChangeAmmoRapidAction, ETriggerEvent::Triggered, this, &AGun::SetAmmoRapid);
		}
	}
}

/** Performs a ray casts, returns true if hit is registered */
bool AGun::GunTrace(FHitResult& HitResult, FVector& ShotDirection)
{
	AController* OwnerController = GetOwnerController();
	if(OwnerController == nullptr)
	{
		
		return false;
	}
	
	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	ShotDirection = -Rotation.Vector();
	FVector End = Location + Rotation.Vector() * MaxRange;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Character);
	return GetWorld()->LineTraceSingleByChannel(HitResult,Location,End,ECollisionChannel::ECC_GameTraceChannel1, Params);
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
	case Piercing:
		Total = *(Character->GetAmmoCountMap()->Find(Piercing));
		if(Total==0)
		{
			return;
		}
		if( Total + PiercingMag <= 12 )
		{
			Character->GetAmmoCountMap()->Emplace(Piercing, 0);
			SetPiercingMag(Total+PiercingMag);
		} else
		{
			Character->GetAmmoCountMap()->Emplace(Piercing,(Total - 12) + PiercingMag);
			SetPiercingMag(12);
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
	default:break;
	}
}

void AGun::SetRegularMag(int32 Size)
{
	RegularMag = Size;
}
void AGun::SetPiercingMag(int32 Size)
{
	PiercingMag = Size;
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


void AGun::SetAmmoRegular()
{
	CurrentAmmoType=AMMO_TYPES::Regular;
}
void AGun::SetAmmoExplosive()
{
	CurrentAlternateAmmoType=AMMO_TYPES::Explosive;
}
void AGun::SetAmmoFlare()
{
	CurrentAlternateAmmoType=AMMO_TYPES::Flare;
}
void AGun::SetAmmoPiercing()
{
	CurrentAmmoType=AMMO_TYPES::Piercing;
}
void AGun::SetAmmoRapid()
{
	CurrentAmmoType=AMMO_TYPES::Rapid;
	UE_LOG(LogTemp,Display,TEXT("Ammo swapped"));
}


AMMO_TYPES AGun::GetCurrentAmmoType()
{
	return CurrentAmmoType;
}
AMMO_TYPES AGun::GetCurrentAlternateAmmoType()
{
	return CurrentAlternateAmmoType;
}

void AGun::FireRegular(FHitResult& Hit, FVector& ShotDirection)
{
	// Decrease ammo count by 1
	//Character->GetAmmoCountMap()->Emplace(Regular, *(Character->GetAmmoCountMap()->Find(Regular)) - 1);
	RegularMag--;
	if(GunTrace(Hit, ShotDirection))
	{
		DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Cyan,true,5);
		AActor* HitActor = Hit.GetActor();
		if(HitActor != nullptr)
		{
			if (ImpactSoundRegularAmmo != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundRegularAmmo, Hit.Location);
			}
			if (ImpactEffectRegularAmmo != nullptr)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffectRegularAmmo, Hit.Location, ShotDirection.Rotation());
			}
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, GetOwnerController(), GetOwner());;
		}
		FireTriggerEvent(Hit, ShotDirection);
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

void AGun::FireExplosive(FHitResult& Hit, FVector& ShotDirection)
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
	//if(GunTrace(Hit, ShotDirection))
	//{
	//	
	//	DrawDebugSphere(GetWorld(),Hit.Location,120,10,FColor::Red,true,5);
	//	
	//	AActor* HitActor = Hit.GetActor();
	//	if(HitActor != nullptr)
	//	{
	//		if (ImpactSoundExplosiveAmmo != nullptr)
	//		{
	//			UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundExplosiveAmmo, Hit.Location);
	//		}
	//		if (ImpactEffectExplosiveAmmo != nullptr)
	//		{
	//			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffectExplosiveAmmo, Hit.Location, ShotDirection.Rotation());
	//		}
	//		UGameplayStatics::ApplyRadialDamageWithFalloff(
	//			GetWorld(),
	//			80.f,
	//			20.f,
	//			Hit.Location,
	//			60.f,
	//			120.f,
	//			1.f,
	//			nullptr,
	//			{},
	//			Character,
	//			GetOwnerController(),
	//			ECC_Visibility
	//			);
	//		//AddRadialForce(Hit.Location, 60.f, 1000.f, RIF_Linear, false);
	//		//AddRadialImpulse(Hit.Location, 60.f, 1000.f, RIF_Linear, false);
	//	}
	//}

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

void AGun::FireFlare(FHitResult& Hit, FVector& ShotDirection)
{
	//Character->GetAmmoCountMap()->Emplace(Flare, *(Character->GetAmmoCountMap()->Find(Flare)) - 1);
	FlareMag--;
	if(GunTrace(Hit, ShotDirection))
	{
		
		DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Yellow,true,5);
		AActor* HitActor = Hit.GetActor();
		if(HitActor != nullptr)
		{
			if (ImpactSoundFlareAmmo != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundFlareAmmo, Hit.Location);
			}
			if (ImpactEffectFlareAmmo != nullptr)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffectFlareAmmo, Hit.Location, ShotDirection.Rotation());
			}
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, GetOwnerController(), GetOwner());;
		}
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

void AGun::FirePiercing(FHitResult& Hit, FVector& ShotDirection)
{
	//Character->GetAmmoCountMap()->Emplace(Piercing, *(Character->GetAmmoCountMap()->Find(Piercing)) - 1);
	PiercingMag--;
	if(GunTrace(Hit, ShotDirection))
	{
		
		DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Purple,true,5);
		AActor* HitActor = Hit.GetActor();
		if(HitActor != nullptr)
		{
			if (ImpactSoundPiercingAmmo != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundPiercingAmmo, Hit.Location);
			}
			if (ImpactEffectPiercingAmmo != nullptr)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffectPiercingAmmo, Hit.Location, ShotDirection.Rotation());
			}
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, GetOwnerController(), GetOwner());;
		}
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

void AGun::FireRapid(FHitResult& Hit, FVector& ShotDirection)
{
	UE_LOG(LogTemp,Display,TEXT("Fire rapidly"));
	// Decrease ammo count by 1
	RapidMag--;
	if(GunTraceInaccurate(Hit, ShotDirection))
	{
		DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Orange,true,5);
		AActor* HitActor = Hit.GetActor();
		if(HitActor != nullptr)
		{
			if (ImpactSoundRegularAmmo != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundRegularAmmo, Hit.Location);
			}
			if (ImpactEffectRegularAmmo != nullptr)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffectRegularAmmo, Hit.Location, ShotDirection.Rotation());
			}
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, GetOwnerController(), GetOwner());;
		}
		FireTriggerEvent(Hit, ShotDirection);
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

bool AGun::GunTraceInaccurate(FHitResult& HitResult, FVector& ShotDirection)
{
	AController* OwnerController = GetOwnerController();
	if(OwnerController == nullptr)
	{
		return false;
	}
	
	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	Rotation = RandomRotator(Rotation.Pitch,Rotation.Yaw,Rotation.Roll, InaccuracyModifier);
	ShotDirection = -Rotation.Vector();
	FVector End = Location + Rotation.Vector()*MaxRange;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Character);
	return GetWorld()->LineTraceSingleByChannel(HitResult,Location,End,ECollisionChannel::ECC_GameTraceChannel1, Params);
}

FRotator AGun::RandomRotator(float Pitch, float Yaw, float Roll, float Interval) const
{
	const float NewPitch = FMath::FRandRange(Pitch,Pitch+Interval);
	const float NewYaw = FMath::FRandRange(Yaw-Interval,Yaw+Interval);
	//const float NewRoll = FMath::FRandRange(Roll-Interval,Roll+Interval);
	return FRotator(NewPitch,NewYaw,Roll);
}






